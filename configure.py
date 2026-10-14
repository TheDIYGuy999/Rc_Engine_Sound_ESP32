#!/usr/bin/env python3
"""
DIYGuy999 RC_Engine_Sound_ESP32 Configurator
Run from project root: python configure.py
Then open: http://localhost:8080
"""

import json
import os
import re
import subprocess
import sys
import time
import urllib.parse
from http.server import BaseHTTPRequestHandler, HTTPServer
from socketserver import ThreadingMixIn

ROOT = os.path.dirname(os.path.abspath(__file__))
SRC = os.path.join(ROOT, "src")
TOOLS = os.path.join(ROOT, "tools")
PRESETS = os.path.join(ROOT, "presets")
BACKUPS = os.path.join(ROOT, "backups")
PORT = 8080
CONNECTED_PORT = None

FILE_LABELS = {
    "0_generalSettings.h": "General Settings",
    "1_Vehicle.h": "Vehicle",
    "2_Remote.h": "Remote / Communication",
    "3_ESC.h": "ESC",
    "4_Transmission.h": "Transmission",
    "5_Shaker.h": "Shaker Motor",
    "6_Lights.h": "Lights and Neopixel",
    "7_Servos.h": "Servos",
    "8_Sound.h": "Sound",
    "9_Dashboard.h": "Dashboard",
    "10_Trailer.h": "Trailer",
}

CONFIG_FILES = list(FILE_LABELS.keys())


def read_text(path):
    with open(path, "r", encoding="utf-8", errors="replace") as f:
        return f.read()


def write_text(path, text):
    with open(path, "w", encoding="utf-8", newline="\n") as f:
        f.write(text)


def ensure_presets_dir():
  os.makedirs(PRESETS, exist_ok=True)


def safe_preset_token(text):
  s = (text or "").strip()
  s = re.sub(r"[^A-Za-z0-9._-]+", "_", s)
  return s.strip("._-")


def preset_file_path(vehicle_file, preset_name):
  ensure_presets_dir()
  v = safe_preset_token(os.path.splitext(os.path.basename(vehicle_file))[0])
  p = safe_preset_token(preset_name)
  if not v or not p:
    raise ValueError("Invalid vehicle or preset name")
  return os.path.join(PRESETS, "%s__%s.json" % (v, p))


def list_vehicle_presets(vehicle_file):
  ensure_presets_dir()
  v = safe_preset_token(os.path.splitext(os.path.basename(vehicle_file))[0])
  if not v:
    return []
  out = []
  prefix = v + "__"
  for fn in os.listdir(PRESETS):
    if not fn.lower().endswith(".json"):
      continue
    if not fn.startswith(prefix):
      continue
    name = fn[len(prefix) : -5]
    if name:
      out.append(name)
  return sorted(set(out), key=str.lower)


def ensure_backups_dir():
  os.makedirs(BACKUPS, exist_ok=True)


def backup_path_for(vehicle_file):
  """Return the backup file path for a given vehicle .h file."""
  ensure_backups_dir()
  base = os.path.basename(vehicle_file)
  return os.path.join(BACKUPS, base)


def ensure_vehicle_backup(vehicle_file):
  """If no backup exists yet, copy the current vehicle file as the original."""
  bp = backup_path_for(vehicle_file)
  if os.path.exists(bp):
    return  # already have a backup
  src = os.path.join(SRC, "vehicles", os.path.basename(vehicle_file))
  if os.path.isfile(src):
    import shutil
    shutil.copy2(src, bp)


def esc(s):
    return (
        str(s)
        .replace("&", "&amp;")
        .replace("<", "&lt;")
        .replace(">", "&gt;")
        .replace('"', "&quot;")
    )


def should_skip_entry(lines, i, name):
    if "dont_use" in name.lower() or "do_not_use" in name.lower():
        return True

    probe = []
    for offset in (-2, -1, 0):
        j = i + offset
        if 0 <= j < len(lines):
            probe.append(lines[j].lower())
    blob = " ".join(probe)

    if re.search(r"do\s*not\s*use", blob):
        return True
    if re.search(r"don'?t\s*use", blob):
        return True
    if re.search(r"not\s*used", blob):
        return True

    return False


def section_key(text):
    return re.sub(r"[^a-z0-9]+", "_", text.lower()).strip("_")


def clean_comment(text):
    t = re.sub(r"^\s*//+\s*", "", text or "").strip()
    t = re.sub(r"\s*-+$", "", t).strip()
    return t


def simplify_description(desc):
    """Shorten description for display — strip code junk."""
    if not desc:
        return ""
    short = desc
    # Strip C preprocessor and code fragments
    short = re.sub(r'#(?:define|ifdef|ifndef|endif|include)\b[^,;)]*', '', short)
    short = re.sub(r'"[^"]*\.h"', '', short)
    # Strip common verbose patterns
    short = re.sub(r'^(The |Set |Configure |Choose |Adjust |Select )', '', short, flags=re.I)
    short = re.sub(r'\buncomment\b.*?(,|\.|;|$)', '', short, flags=re.I)
    short = re.sub(r'\bcomment\b.*?out.*?(,|\.|;|$)', '', short, flags=re.I)
    short = re.sub(r"don't uncomment.*", '', short, flags=re.I)
    short = re.sub(r'\bif you (want|don).{0,60}?(\.|,|;|!|$)', '', short, flags=re.I)
    short = re.sub(r'\(usually\s*=?[^)]*\)', '', short)
    short = re.sub(r'\(set\s[^)]*\)', '', short, flags=re.I)
    short = re.sub(r'\(use\s[^)]*\)', '', short, flags=re.I)
    short = re.sub(r'\(about\s[^)]*\)', '', short, flags=re.I)
    short = re.sub(r'\(never\s[^)]*\)', '', short, flags=re.I)
    # Clean up leftover punctuation and whitespace
    short = re.sub(r'\s*-+\s*$', '', short)
    short = re.sub(r'\s{2,}', ' ', short).strip(' -,;()')
    if len(short) > 80:
        short = short[:77].rsplit(' ', 1)[0] + '...'
    return short


# Fields that should render as range sliders instead of text inputs.
# Format: "varName": (min, max, step, suffix)
SLIDER_FIELDS = {
    # ── Volumes ──
    "MAX_RPM_PERCENTAGE":              (100, 500, 10, "%"),
    "idleVolumePercentage":            (0, 300, 5, "%"),
    "engineIdleVolumePercentage":      (0, 100, 5, "%"),
    "fullThrottleVolumePercentage":    (0, 300, 5, "%"),
    "revVolumePercentage":             (0, 300, 5, "%"),
    "engineRevVolumePercentage":       (0, 100, 5, "%"),
    "startVolumePercentage":           (0, 300, 5, "%"),
    "turboVolumePercentage":           (0, 300, 5, "%"),
    "turboIdleVolumePercentage":       (0, 100, 5, "%"),
    "fanVolumePercentage":             (0, 300, 5, "%"),
    "fanIdleVolumePercentage":         (0, 100, 5, "%"),
    "chargerVolumePercentage":         (0, 300, 5, "%"),
    "chargerIdleVolumePercentage":     (0, 100, 5, "%"),
    "wastegateVolumePercentage":       (0, 300, 5, "%"),
    "wastegateIdleVolumePercentage":   (0, 100, 5, "%"),
    "dieselKnockVolumePercentage":     (0, 600, 10, "%"),
    "dieselKnockIdleVolumePercentage": (0, 100, 5, "%"),
    "dieselKnockAdaptiveVolumePercentage": (0, 100, 5, "%"),
    "hornVolumePercentage":            (0, 300, 5, "%"),
    "sirenVolumePercentage":           (0, 300, 5, "%"),
    "jakeBrakeVolumePercentage":       (0, 300, 5, "%"),
    "jakeBrakeIdleVolumePercentage":   (0, 100, 5, "%"),
    "brakeVolumePercentage":           (0, 300, 5, "%"),
    "parkingBrakeVolumePercentage":    (0, 300, 5, "%"),
    "shiftingVolumePercentage":        (0, 300, 5, "%"),
    "sound1VolumePercentage":          (0, 300, 5, "%"),
    "reversingVolumePercentage":       (0, 300, 5, "%"),
    "indicatorVolumePercentage":       (0, 300, 5, "%"),
    "couplingVolumePercentage":        (0, 300, 5, "%"),
    "hydraulicPumpVolumePercentage":   (0, 300, 5, "%"),
    "hydraulicFlowVolumePercentage":   (0, 300, 5, "%"),
    "trackRattleVolumePercentage":     (0, 300, 5, "%"),
    "trackRattle2VolumePercentage":    (0, 300, 5, "%"),
    "bucketRattleVolumePercentage":    (0, 300, 5, "%"),
    "outOfFuelVolumePercentage":       (0, 300, 5, "%"),
    # ── Engine tuning ──
    "revSwitchPoint":                  (0, 500, 10, ""),
    "idleEndPoint":                    (0, 500, 10, ""),
    "idleVolumeProportionPercentage":  (0, 100, 5, "%"),
    "dieselKnockInterval":             (0, 40, 1, ""),
    "dieselKnockStartPoint":           (0, 500, 10, ""),
    "jakeBrakeMinRpm":                 (0, 500, 10, ""),
    "chargerStartPoint":               (0, 500, 10, ""),
    "fanStartPoint":                   (0, 500, 10, ""),
    "indicatorOn":                     (0, 100, 5, ""),
    "clutchEngagingPoint":             (0, 500, 10, ""),
    # ── Transmission ──
    "acc":                              (1, 9, 1, ""),
    "dec":                              (1, 9, 1, ""),
    "escRampTimeFirstGear":            (0, 50, 1, ""),
    "escRampTimeSecondGear":           (0, 50, 1, ""),
    "escRampTimeThirdGear":            (0, 50, 1, ""),
    "escBrakeSteps":                   (1, 30, 1, ""),
    "escAccelerationSteps":            (1, 30, 1, ""),
    "NumberOfAutomaticGears":          (1, 6, 1, ""),
    "automaticReverseAccelerationPercentage": (0, 100, 5, "%"),
    "lowRangePercentage":              (0, 100, 5, "%"),
    "maxClutchSlippingRpm":            (0, 500, 10, ""),
    "globalAccelerationPercentage":    (0, 200, 5, "%"),
    # ── ESC ──
    "brakeMargin":                     (0, 50, 1, ""),
    "escPulseSpan":                    (100, 600, 10, ""),
    "escTakeoffPunch":                 (0, 200, 5, ""),
    "escReversePlus":                  (0, 100, 5, ""),
    "crawlerEscRampTime":              (0, 50, 1, ""),
    "directionChangeLimit":            (0, 100, 5, ""),
    "RZ7886_FREQUENCY":                (100, 50000, 500, " Hz"),
    "RZ7886_DRAGBRAKE_DUTY":           (0, 255, 5, ""),
    # ── Track drive ──
    "pwmStrokeChainDriveTopSpeed":     (0, 255, 5, ""),
    "pwmStrokeChainDriveStartRotation": (0, 100, 5, ""),
    "trackRattleIntervalMin":          (0, 500, 10, " ms"),
    "trackRattleIntervalMax":          (0, 2000, 50, " ms"),
    # ── Battery ──
    "CUTOFF_VOLTAGE":                  (0, 50, 1, " V"),
    "FULLY_CHARGED_VOLTAGE":           (0, 50, 1, " V"),
    "RECOVERY_HYSTERESIS":             (0, 10, 1, " V"),
    "RESISTOR_TO_BATTTERY_PLUS":       (0, 200000, 1000, " Ω"),
    "RESISTOR_TO_GND":                 (0, 200000, 1000, " Ω"),
    "DIODE_DROP":                      (0, 10, 1, " V"),
    # ── Shaker ──
    "shakerStart":                     (0, 255, 5, ""),
    "shakerIdle":                      (0, 255, 5, ""),
    "shakerFullThrottle":              (0, 255, 5, ""),
    "shakerStop":                      (0, 255, 5, ""),
    # ── Lights ──
    "NEOPIXEL_COUNT":                  (0, 100, 1, ""),
    "NEOPIXEL_BRIGHTNESS":             (0, 255, 5, ""),
    "MAX_POWER_MILLIAMPS":             (0, 5000, 100, " mA"),
    "neopixelMode":                    (0, 5, 1, ""),
    "cabLightsBrightness":             (0, 255, 5, ""),
    "sideLightsBrightness":            (0, 255, 5, ""),
    "rearlightDimmedBrightness":       (0, 255, 5, ""),
    "rearlightParkingBrightness":      (0, 255, 5, ""),
    "headlightParkingBrightness":      (0, 255, 5, ""),
    "reversingLightBrightness":        (0, 255, 5, ""),
    "fogLightBrightness":              (0, 255, 5, ""),
    # ── Remote ──
    "pulseNeutral":                    (0, 100, 5, ""),
    "pulseSpan":                       (100, 600, 10, ""),
    # ── Servos ──
    "SERVO_FREQUENCY":                 (50, 400, 10, " Hz"),
    "STEERING_RAMP_TIME":              (0, 2000, 50, " ms"),
    "CH1_RAMP_TIME":                   (0, 2000, 50, " ms"),
    "CH2_RAMP_TIME":                   (0, 2000, 50, " ms"),
    "CH3_RAMP_TIME":                   (0, 2000, 50, " ms"),
    "CH4_RAMP_TIME":                   (0, 2000, 50, " ms"),
    # ── Sound ──
    "numberOfVolumeSteps":             (1, 10, 1, ""),
    "masterVolumeCrawlerThreshold":    (0, 255, 5, ""),
    # ── Dashboard ──
    "dashRotation":                    (0, 3, 1, ""),
    "MAX_REAL_SPEED":                  (0, 300, 10, ""),
    "RPM_MAX":                         (0, 10000, 500, ""),
}

# Friendly display names for cryptic C++ variable names
FRIENDLY_NAMES = {
    # ── Vehicle Sound Volumes ──
    "startVolumePercentage": "Engine Start Volume",
    "idleVolumePercentage": "Idle Volume",
    "engineIdleVolumePercentage": "Idle Engine Volume",
    "fullThrottleVolumePercentage": "Full Throttle Volume",
    "revVolumePercentage": "Rev Volume",
    "engineRevVolumePercentage": "Rev Engine Volume",
    "revSwitchPoint": "Rev Crossfade Start",
    "idleEndPoint": "Rev Full Point",
    "idleVolumeProportionPercentage": "Idle/Rev Mix Ratio",
    "jakeBrakeVolumePercentage": "Jake Brake Volume",
    "jakeBrakeIdleVolumePercentage": "Jake Brake Idle Volume",
    "jakeBrakeMinRpm": "Jake Brake Min RPM",
    "dieselKnockVolumePercentage": "Diesel Knock Volume",
    "dieselKnockIdleVolumePercentage": "Diesel Knock Idle Volume",
    "dieselKnockInterval": "Diesel Knock Spacing",
    "dieselKnockStartPoint": "Diesel Knock Start RPM",
    "dieselKnockAdaptiveVolumePercentage": "Knock Follow-up Volume",
    "turboVolumePercentage": "Turbo Whistle Volume",
    "turboIdleVolumePercentage": "Turbo Idle Volume",
    "chargerVolumePercentage": "Supercharger Volume",
    "chargerIdleVolumePercentage": "Supercharger Idle Volume",
    "chargerStartPoint": "Supercharger Start RPM",
    "wastegateVolumePercentage": "Wastegate Volume",
    "wastegateIdleVolumePercentage": "Wastegate Idle Volume",
    "fanVolumePercentage": "Cooling Fan Volume",
    "fanIdleVolumePercentage": "Fan Idle Volume",
    "fanStartPoint": "Fan Start RPM",
    "hornVolumePercentage": "Horn Volume",
    "sirenVolumePercentage": "Siren Volume",
    "brakeVolumePercentage": "Air Brake Volume",
    "parkingBrakeVolumePercentage": "Parking Brake Volume",
    "shiftingVolumePercentage": "Gear Shift Volume",
    "sound1VolumePercentage": "Extra Sound Volume",
    "reversingVolumePercentage": "Reverse Beep Volume",
    "indicatorVolumePercentage": "Indicator Click Volume",
    "indicatorOn": "Indicator Steering Threshold",
    "couplingVolumePercentage": "Coupling Sound Volume",
    "hydraulicPumpVolumePercentage": "Hydraulic Pump Volume",
    "hydraulicFlowVolumePercentage": "Hydraulic Flow Volume",
    "trackRattleVolumePercentage": "Track Rattle Volume",
    "trackRattle2VolumePercentage": "Track Rattle 2 Volume",
    "bucketRattleVolumePercentage": "Bucket Rattle Volume",
    # ── Transmission ──
    "clutchEngagingPoint": "Clutch Engage RPM",
    "MAX_RPM_PERCENTAGE": "Max RPM %",
    "acc": "Acceleration Speed",
    "dec": "Deceleration Speed",
    "escRampTimeFirstGear": "1st Gear Ramp Time",
    "escRampTimeSecondGear": "2nd Gear Ramp Time",
    "escRampTimeThirdGear": "3rd Gear Ramp Time",
    "escBrakeSteps": "Brake Speed",
    "escAccelerationSteps": "Acceleration Steps",
    "NumberOfAutomaticGears": "Auto Gears Count",
    "automaticReverseAccelerationPercentage": "Reverse Accel %",
    "lowRangePercentage": "Low Range Speed %",
    "maxClutchSlippingRpm": "Max Clutch Slip RPM",
    # ── Track Drive ──
    "pwmStrokeChainDriveTopSpeed": "Track Top Speed PWM",
    "pwmStrokeChainDriveStartRotation": "Track Start Deadband",
    "trackRattleIntervalMin": "Track Rattle Fast (ms)",
    "trackRattleIntervalMax": "Track Rattle Slow (ms)",
    # ── ESC ──
    "brakeMargin": "ESC Brake Dead Zone",
    "escPulseSpan": "ESC Power Range",
    "escTakeoffPunch": "ESC Takeoff Punch",
    "escReversePlus": "ESC Reverse Boost",
    "crawlerEscRampTime": "Crawler Ramp Time",
    "globalAccelerationPercentage": "Global Accel %",
    "directionChangeLimit": "Direction Change Max",
    "RZ7886_FREQUENCY": "Motor Driver Frequency",
    "RZ7886_DRAGBRAKE_DUTY": "Drag Brake Strength",
    # ── Battery ──
    "CUTOFF_VOLTAGE": "Battery Cutoff Voltage",
    "FULLY_CHARGED_VOLTAGE": "Full Charge Voltage",
    "RECOVERY_HYSTERESIS": "Recovery Hysteresis",
    "RESISTOR_TO_BATTTERY_PLUS": "Divider R to Batt+",
    "RESISTOR_TO_GND": "Divider R to Ground",
    "DIODE_DROP": "Diode Voltage Drop",
    "outOfFuelVolumePercentage": "Low Battery Alert Vol",
    # ── Shaker Motor ──
    "shakerStart": "Shaker Start Power",
    "shakerIdle": "Shaker Idle Power",
    "shakerFullThrottle": "Shaker Full Throttle",
    "shakerStop": "Shaker Stop Power",
    # ── Lights ──
    "NEOPIXEL_COUNT": "Neopixel LED Count",
    "NEOPIXEL_BRIGHTNESS": "Neopixel Brightness",
    "MAX_POWER_MILLIAMPS": "Neopixel Max Power (mA)",
    "neopixelMode": "Neopixel Mode",
    "cabLightsBrightness": "Cab Light Brightness",
    "sideLightsBrightness": "Side Light Brightness",
    "rearlightDimmedBrightness": "Tail Light Dim Brightness",
    "rearlightParkingBrightness": "Tail Light Park Brightness",
    "headlightParkingBrightness": "Headlight Park Brightness",
    "reversingLightBrightness": "Reverse Light Brightness",
    "fogLightBrightness": "Fog Light Brightness",
    # ── Remote ──
    "sbusBaud": "SBUS Baud Rate",
    "sbusFailsafeTimeout": "Failsafe Timeout (ms)",
    "pulseNeutral": "Neutral Zone Width",
    "pulseSpan": "Stick Travel Range",
    # ── Servos ──
    "SERVO_FREQUENCY": "Servo Update Rate (Hz)",
    "STEERING_RAMP_TIME": "Steering Smoothing (ms)",
    "CH1_RAMP_TIME": "CH1 Smoothing (ms)",
    "CH2_RAMP_TIME": "CH2 Smoothing (ms)",
    "CH3_RAMP_TIME": "CH3 Smoothing (ms)",
    "CH4_RAMP_TIME": "CH4 Smoothing (ms)",
    # ── Sound Settings ──
    "numberOfVolumeSteps": "Volume Steps",
    "masterVolumeCrawlerThreshold": "Crawler Volume Trigger",
    # ── Dashboard ──
    "dashRotation": "Display Orientation",
    "MAX_REAL_SPEED": "Max Speed Display",
    "RPM_MAX": "Max RPM Display",
    # ── General ──
    "eeprom_id": "EEPROM Profile ID",
    "cpType": "WiFi Power Level",
    "default_ssid": "WiFi Network Name",
    "default_password": "WiFi Password",
    # ── Define Flags (on/off toggles) ──
    "REV_SOUND": "Separate Rev Sound",
    "JAKE_BRAKE_SOUND": "Jake Brake Sound",
    "V8": "V8 Engine Mode",
    "V2": "V2 Engine Mode",
    "GEARBOX_WHINING": "Gearbox Whine Sound",
    "LED_INDICATORS": "LED Turn Signals",
    "INDICATOR_DIR": "Swap Indicator Direction",
    "COUPLING_SOUND": "Trailer Coupling Sounds",
    "EXCAVATOR_MODE": "Excavator Mode",
    "HYDROSTATIC_TRACK_MOTORS": "Hydrostatic Track Motor",
    "TRACK_RATTLE_2": "Second Track Rattle",
    "XENON_LIGHTS": "Xenon Headlight Flash",
    "SEPARATE_FULL_BEAM": "Separate High Beam Pin",
    "doubleFlashBlueLight": "Double Flash Emergency",
    "TRACKED_MODE": "Tracked Vehicle Mode",
    "VIRTUAL_3_SPEED": "Virtual 3-Speed Gearbox",
    "VIRTUAL_16_SPEED_SEQUENTIAL": "Virtual 16-Speed Sequential",
    "OVERDRIVE": "Overdrive Gear",
    "SEMI_AUTOMATIC": "Semi-Automatic Mode",
    "MODE1_SHIFTING": "Mode 1 Gear Shifting",
    "TRANSMISSION_NEUTRAL": "Transmission Neutral",
    "DOUBLE_CLUTCH": "Double Clutch Shifting",
    "HIGH_SLIPPINGPOINT": "High Clutch Slip Point",
    "QUICRUN_FUSION": "Quicrun Fusion ESC Fix",
    "QUICRUN_16BL30": "Quicrun 16BL30 Fix",
    "ESC_DIR": "Reverse Motor Direction",
    "HYDROSTATIC_MODE": "Hydrostatic Drive Mode",
    "RZ7886_DRIVER_MODE": "RZ7886 Motor Driver",
    "BATTERY_PROTECTION": "Battery Low-Voltage Cutoff",
    "GT_POWER_STOCK": "GT-Power Brass Weight",
    "GT_POWER_PLASTIC": "GT-Power Plastic Weight",
    "NEOPIXEL_ENABLED": "Neopixel LEDs Enabled",
    "NEOPIXEL_ON_CH4": "Neopixel on CH4 Header",
    "NEOPIXEL_HIGHBEAM": "Neopixel as High Beam",
    "THIRD_BRAKELIGHT": "3rd Brake Light on Pin 32",
    "ROTATINGBEACON_ON_B1": "Rotating Beacon on B1",
    "INDICATOR_TOGGLING_MODE": "Indicator Toggle Mode",
    "DEBUG": "Debug Mode",
    "CHANNEL_DEBUG": "Channel Debug",
    "ESC_DEBUG": "ESC Debug",
    "AUTO_TRANS_DEBUG": "Auto Transmission Debug",
    "MANUAL_TRANS_DEBUG": "Manual Transmission Debug",
    "TRACKED_DEBUG": "Tracked Vehicle Debug",
    "SERVO_DEBUG": "Servo Debug",
    "ESPNOW_DEBUG": "ESP-NOW Debug",
    "CORE_DEBUG": "Core Debug",
    "ERASE_EEPROM_ON_BOOT": "Erase EEPROM on Boot",
    "ENABLE_WIRELESS": "Enable Wireless (ESP-Now/WiFi)",
    "USE_CSS": "Simple CSS Styling",
    "MODERN_CSS": "Modern CSS with Scaling",
    "WEMOS_D1_MINI_ESP32": "WeMos D1 Mini ESP32 Board",
    # ── Remote Control Profiles ──
    "FLYSKY_FS_I6X": "FlySky FS-i6X",
    "FLYSKY_FS_I6S": "FlySky FS-i6S",
    "FLYSKY_FS_I6S_LOADER": "FlySky FS-i6S (Loader)",
    "FLYSKY_FS_I6S_DOZER": "FlySky FS-i6S (Dozer)",
    "FLYSKY_FS_I6S_EXCAVATOR": "FlySky FS-i6S (Excavator)",
    "FLYSKY_FS_I6S_EXCAVATOR_TEST": "FlySky FS-i6S (Excavator Test)",
    "FLYSKY_GT5": "FlySky GT5 / Reely GT6",
    "FRSKY_TANDEM_EXCAVATOR": "FrSky Tandem (Excavator)",
    "FRSKY_TANDEM_HARMONY_LOADER": "FrSky Tandem (Loader)",
    "FRSKY_TANDEM_CRANE": "FrSky Tandem (Crane)",
    "RGT_EX86100": "RGT EX86100 / MT-305",
    "GRAUPNER_MZ_12": "Graupner MZ-12 PRO",
    "MICRO_RC": "Micro RC (Car Style)",
    "MICRO_RC_STICK": "Micro RC (Stick Style)",
    "PROTOTYPE_36": "36-Pin Prototype Board",
    # ── Communication Protocol ──
    "SBUS_COMMUNICATION": "SBUS Protocol",
    "EMBEDDED_SBUS": "Embedded SBUS Code",
    "IBUS_COMMUNICATION": "iBUS Protocol",
    "SUMD_COMMUNICATION": "SUMD Protocol",
    "PPM_COMMUNICATION": "PPM Protocol",
    # ── Driving Aids ──
    "EXPONENTIAL_THROTTLE": "Exponential Throttle Curve",
    "EXPONENTIAL_STEERING": "Exponential Steering Curve",
    "CHANNEL_AVERAGING": "Channel Signal Averaging",
    "AUTO_LIGHTS": "Auto Lights with Engine",
    "AUTO_ENGINE_ON_OFF": "Auto Engine Start/Stop",
    "AUTO_INDICATORS": "Auto Turn Signals",
    # ── Channel Mapping ──
    "STEERING": "Steering Channel",
    "GEARBOX": "Gearbox Channel",
    "THROTTLE": "Throttle Channel",
    "HORN": "Horn Channel",
    "FUNCTION_R": "Function Right Channel",
    "FUNCTION_L": "Function Left Channel",
    "POT2": "Pot 2 Channel",
    "MODE1": "Mode 1 Channel",
    "MODE2": "Mode 2 Channel",
    "MOMENTARY1": "Momentary 1 Channel",
    "HAZARDS": "Hazards Channel",
    "INDICATOR_LEFT": "Left Indicator Channel",
    "INDICATOR_RIGHT": "Right Indicator Channel",
    "CH_14": "Channel 14",
    "CH_15": "Channel 15",
    "CH_16": "Channel 16",
    # ── Servo Configs ──
    "SERVOS_DEFAULT": "Default Servo Config",
    "SERVOS_ACTROS": "Actros Servo Config",
    "SERVOS_C34": "C34 Servo Config",
    "SERVOS_CRANE": "Crane Servo Config",
    "SERVOS_EXCAVATOR": "Excavator Servo Config",
    "SERVOS_HYDRAULIC_EXCAVATOR": "Hydraulic Excavator Servos",
    "SERVOS_KING_HAULER": "King Hauler Servo Config",
    "SERVOS_LANDY_DOUBLE_EAGLE": "Landy Double Eagle Servos",
    "SERVOS_LANDY_MN_MODEL": "Landy MN Model Servos",
    "SERVOS_MECCANO_DUMPER": "Meccano Dumper Servos",
    "SERVOS_OPEN_RC_TRACTOR": "Open RC Tractor Servos",
    "SERVOS_RACING_TRUCK": "Racing Truck Servos",
    "SERVOS_RGT_EX86100": "RGT EX86100 Servos",
    "SERVOS_URAL": "Ural Servo Config",
    # ── Dashboard ──
    "SPI_DASHBOARD": "SPI Dashboard Display",
    "FREVIC_DASHBOARD": "Frevic Dashboard Layout",
    # ── Sound Toggles ──
    "NO_SIREN": "Disable Siren Sound",
    "NO_INDICATOR_SOUND": "Disable Indicator Sound",
    # ── Trailer ──
    "CH3_BEACON": "Beacon on CH3 Output",
    "MODE2_TRAILER_UNLOCKING": "Mode 2: Trailer Unlock",
    "MODE2_WINCH": "Mode 2: Winch Control",
    "NO_WINCH_DELAY": "Instant Winch Response",
    "MODE2_HYDRAULIC": "Mode 2: Hydraulic Valve",
    "PINGON_MODE": "Pingon Wheel Lift",
    "TRAILER_LIGHTS_TRAILER_PRESENCE_SWITCH_DEPENDENT": "Trailer Light Detect Switch",
    # ── Bool Variables ──
    "noCabLights": "Skip Cab Lights",
    "noFogLights": "Skip Fog Lights",
    "xenonLights": "Xenon Flash Effect",
    "flickeringWileCranking": "Flicker While Cranking",
    "ledIndicators": "Snap-On Indicators",
    "swap_L_R_indicators": "Swap L/R Indicators",
    "indicatorsAsSidemarkers": "Indicators as Side Markers",
    "separateFullBeam": "Separate High Beam",
    "flashingBlueLight": "Double Flash Emergency",
    "hazardsWhile5thWheelUnlocked": "Hazards on 5th Wheel Unlock",
    "boomDownwardsHydraulic": "Boom Down Hydraulic Sound",
    "reverseBoomSoundDirection": "Reverse Boom Direction",
    "sbusInverted": "SBUS Non-Inverted",
    "defaultUseTrailer1": "Enable Trailer 1",
    "defaultUseTrailer2": "Enable Trailer 2",
    "defaultUseTrailer3": "Enable Trailer 3",
    "automatic": "Automatic Transmission",
    "doubleClutch": "Double Clutch Mode",
    "shiftingAutoThrottle": "Auto Throttle on Shift",
}

# User-friendly descriptions (shown in the hint column)
FRIENDLY_DESCRIPTIONS = {
    # ── Vehicle Sound Volumes ──
    "startVolumePercentage": "How loud the engine startup sound plays. 100% is normal.",
    "idleVolumePercentage": "Overall idle sound loudness. Higher = louder at idle.",
    "engineIdleVolumePercentage": "How much engine sound you hear at idle vs throttle. Max 100%.",
    "fullThrottleVolumePercentage": "Volume boost at wide open throttle. Applies to rev too.",
    "revVolumePercentage": "How loud the revving/acceleration sound plays.",
    "engineRevVolumePercentage": "Engine volume while revving (throttle dependent). Max 100%.",
    "revSwitchPoint": "RPM where idle sound starts fading and rev sound takes over.",
    "idleEndPoint": "RPM where it's 100% rev sound and 0% idle. Must be higher than Rev Crossfade Start.",
    "idleVolumeProportionPercentage": "Below Rev Crossfade — how much idle vs rev sound. 100 = all idle.",
    "jakeBrakeVolumePercentage": "Max volume for the engine braking / jake brake effect.",
    "jakeBrakeIdleVolumePercentage": "Minimum jake brake volume at low RPM.",
    "jakeBrakeMinRpm": "RPM below which jake brake sound won't play.",
    "dieselKnockVolumePercentage": "Diesel ignition knock overlay volume. 200-600% is normal.",
    "dieselKnockIdleVolumePercentage": "Knock sound volume while idling. Usually around 20%.",
    "dieselKnockInterval": "Controls knock frequency. Lower = more frequent knocks.",
    "dieselKnockStartPoint": "Knock volume increases above this RPM. 0 for always, ~250 for open pipes.",
    "dieselKnockAdaptiveVolumePercentage": "Volume of follow-up knocks in each cycle (50% is typical).",
    "turboVolumePercentage": "Turbo whistle volume. 0 to disable. ~70% is normal.",
    "turboIdleVolumePercentage": "Minimum turbo volume at idle RPM. ~10% is normal.",
    "chargerVolumePercentage": "Supercharger whine volume. 0 to disable.",
    "chargerIdleVolumePercentage": "Supercharger volume at idle. ~10% is normal.",
    "chargerStartPoint": "RPM above which supercharger sound starts. ~10 is typical.",
    "wastegateVolumePercentage": "Turbo blowoff valve pop volume. ~70% is normal.",
    "wastegateIdleVolumePercentage": "How sensitive the blowoff is to throttle drops.",
    "fanVolumePercentage": "Cooling fan volume. 0 to disable. ~250% for some trucks.",
    "fanIdleVolumePercentage": "Fan volume at low RPM. ~10% is normal.",
    "fanStartPoint": "RPM where fan sound kicks in. 0 for always on.",
    "hornVolumePercentage": "Horn sound volume.",
    "sirenVolumePercentage": "Siren or secondary horn volume.",
    "brakeVolumePercentage": "Air brake hiss volume when braking.",
    "parkingBrakeVolumePercentage": "Parking brake engagement sound volume.",
    "shiftingVolumePercentage": "Pneumatic gear shifting sound volume.",
    "sound1VolumePercentage": "Extra sound (like door opening) volume.",
    "reversingVolumePercentage": "Reverse beeping volume. ~70% is normal.",
    "indicatorVolumePercentage": "Turn signal tick-tock volume.",
    "indicatorOn": "Steering angle that triggers turn signals. Higher = less sensitive.",
    "couplingVolumePercentage": "Trailer coupling/uncoupling sound volume.",
    "hydraulicPumpVolumePercentage": "Hydraulic pump whine volume (excavators). ~120%.",
    "hydraulicFlowVolumePercentage": "Hydraulic fluid flowing sound volume.",
    "trackRattleVolumePercentage": "Track chain rattle volume.",
    "trackRattle2VolumePercentage": "Second track rattle, plays with a delay after the first.",
    "bucketRattleVolumePercentage": "Excavator bucket rattle volume.",
    # ── Engine & Transmission ──
    "clutchEngagingPoint": "RPM where clutch grabs and motor starts moving the vehicle.",
    "MAX_RPM_PERCENTAGE": "Top RPM as a % of idle. 200 for diesels, 400 for gas engines.",
    "acc": "How fast RPM climbs. 1 = slow (locomotive), 9 = fast (trophy truck).",
    "dec": "How fast RPM drops. 1 = slow, 5 = fast.",
    "automatic": "Enable automatic transmission with torque converter simulation.",
    "NumberOfAutomaticGears": "Number of gears for automatic mode.",
    "doubleClutch": "Double-clutch shifting (old manual transmissions). Don't use with automatic.",
    "shiftingAutoThrottle": "Auto-blip throttle during shifts (for Tamiya 3-speed gearboxes).",
    "escRampTimeFirstGear": "1st gear acceleration smoothness. Lower = faster response (15-25).",
    "escRampTimeSecondGear": "2nd gear ramp time. ~50 for semi trucks, ~80 for automatic.",
    "escRampTimeThirdGear": "3rd gear ramp time. ~75 is typical.",
    "escBrakeSteps": "How fast the ESC brakes. Lower = softer braking (20-30).",
    "escAccelerationSteps": "ESC acceleration responsiveness. 2-3 is normal.",
    "automaticReverseAccelerationPercentage": "Reverse speed in auto mode, as % of forward. 100 = same.",
    "lowRangePercentage": "Low range limits max speed to this %. ~58% for WPL trucks.",
    "maxClutchSlippingRpm": "RPM cap for clutch slip effect. 250 normal, 500 for hydrostatic.",
    # ── Track Drive ──
    "pwmStrokeChainDriveTopSpeed": "PWM value where tracks reach max speed. ~1100 typical.",
    "pwmStrokeChainDriveStartRotation": "Dead zone offset before tracks start moving. ~68.",
    "trackRattleIntervalMin": "Track rattle repeat speed at top speed (ms). Don't go below sample length.",
    "trackRattleIntervalMax": "Track rattle repeat speed at slowest speed (ms).",
    # ── ESC ──
    "brakeMargin": "Dead zone around ESC neutral to avoid drag brake. 0-20, never above 20.",
    "escPulseSpan": "ESC signal range. 500 = full power. Lower = less power.",
    "escTakeoffPunch": "Extra punch around neutral for smooth takeoff.",
    "escReversePlus": "Extra speed boost in reverse.",
    "crawlerEscRampTime": "ESC ramp time in crawler mode. ~10 is typical.",
    "globalAccelerationPercentage": "Scales all acceleration. 100% normal, 200% doubles it.",
    "directionChangeLimit": "Max throttle allowed during auto direction changes. ~80.",
    "RZ7886_FREQUENCY": "PWM frequency for RZ7886 motor driver. 500Hz recommended.",
    "RZ7886_DRAGBRAKE_DUTY": "Drag brake strength 0-100%. How much motor resists when coasting.",
    # ── Battery ──
    "CUTOFF_VOLTAGE": "Voltage per LiPo cell to cut off ESC. 3.3V is safe.",
    "FULLY_CHARGED_VOLTAGE": "Full charge voltage per LiPo cell. Usually 4.2V.",
    "RECOVERY_HYSTERESIS": "Voltage buffer before recovery. ~0.2V prevents flicker.",
    "RESISTOR_TO_BATTTERY_PLUS": "Voltage divider top resistor value in ohms.",
    "RESISTOR_TO_GND": "Voltage divider bottom resistor value in ohms.",
    "DIODE_DROP": "Fine-tune voltage reading for diode in circuit.",
    "outOfFuelVolumePercentage": "Volume of low battery warning sound.",
    # ── Shaker Motor ──
    "shakerStart": "Vibration motor power during engine crank (0-255). ~100.",
    "shakerIdle": "Vibration motor power at idle (0-255). ~49.",
    "shakerFullThrottle": "Vibration motor power at full throttle (0-255). ~40.",
    "shakerStop": "Vibration motor power during engine shutdown (0-255). ~60.",
    # ── Lights ──
    "NEOPIXEL_COUNT": "How many WS2812 LEDs in the strip.",
    "NEOPIXEL_BRIGHTNESS": "LED strip brightness 0-255. Higher = brighter but more power.",
    "MAX_POWER_MILLIAMPS": "Power limit for LEDs in milliamps. 100mA is safe.",
    "neopixelMode": "LED animation: 1=Demo, 2=Knight Rider, 3=Blue Light, 4=Running.",
    "cabLightsBrightness": "Interior cabin light brightness (0-255).",
    "sideLightsBrightness": "Side marker light brightness (0-255).",
    "rearlightDimmedBrightness": "Tail light brightness when NOT braking. ~30.",
    "rearlightParkingBrightness": "Tail light brightness in parking mode. 0 or ~5.",
    "headlightParkingBrightness": "Headlight brightness in parking mode. 0 or ~5.",
    "reversingLightBrightness": "Reverse light brightness (0-255). ~140.",
    "fogLightBrightness": "Fog light brightness (0-255). ~255.",
    # ── Remote ──
    "sbusBaud": "SBUS serial speed. 100000 is standard. Try 96000-104000 if issues.",
    "sbusFailsafeTimeout": "Time in ms before failsafe kicks in if signal lost.",
    "pulseNeutral": "Deadzone around center stick (1500\u00b5s \u00b1 this value).",
    "pulseSpan": "Total stick travel range. 480 is standard.",
    # ── Servos ──
    "SERVO_FREQUENCY": "How often servos get updated. 50Hz is standard, 100Hz for digital.",
    "STEERING_RAMP_TIME": "Steering servo smoothing in ms. 0 for instant response.",
    "CH1_RAMP_TIME": "CH1 servo movement smoothing in ms.",
    "CH2_RAMP_TIME": "CH2 servo movement smoothing in ms.",
    "CH3_RAMP_TIME": "CH3 servo movement smoothing in ms.",
    "CH4_RAMP_TIME": "CH4 servo movement smoothing in ms.",
    # ── Sound Settings ──
    "numberOfVolumeSteps": "How many volume levels the remote cycles through.",
    "masterVolumeCrawlerThreshold": "Volume level that activates crawler mode.",
    # ── Dashboard ──
    "dashRotation": "Screen orientation. 1 = flipped, 3 = normal.",
    "MAX_REAL_SPEED": "Top speed shown on the dashboard speedometer.",
    "RPM_MAX": "Max RPM shown on the dashboard tachometer.",
    # ── General ──
    "eeprom_id": "Profile ID stored in EEPROM (1-255). Change to reset settings.",
    "cpType": "WiFi radio power level for wireless features.",
    "default_ssid": "WiFi network name the board connects to.",
    "default_password": "WiFi password.",
    # ── Define Flags (on/off toggles) ──
    "REV_SOUND": "Use a separate revving sound that crossfades with idle.",
    "JAKE_BRAKE_SOUND": "Enable engine braking sound effect.",
    "V8": "V8 engine: louder knocks on cylinders 4 and 8.",
    "V2": "V2 engine (Harley style): louder first 2 of 4 pulses.",
    "GEARBOX_WHINING": "Use fan sound pin for gearbox whine instead (silent in neutral).",
    "LED_INDICATORS": "Turn signals switch instantly instead of blinking smoothly.",
    "INDICATOR_DIR": "Swap left/right indicator direction.",
    "COUPLING_SOUND": "Play sounds when coupling/uncoupling trailer.",
    "EXCAVATOR_MODE": "Enable excavator features (boom, tracks, hydraulics).",
    "HYDROSTATIC_TRACK_MOTORS": "Hydraulic pump sound changes with driving speed.",
    "TRACK_RATTLE_2": "Play a second track rattle with a delay for realistic effect.",
    "XENON_LIGHTS": "Headlights flash briefly when turning on (xenon effect).",
    "SEPARATE_FULL_BEAM": "High beam on its own pin (roof light output).",
    "doubleFlashBlueLight": "True = double-flash emergency lights. False = rotating beacon.",
    "TRACKED_MODE": "Tank/excavator mode: dual throttle on CH2 + CH3.",
    "VIRTUAL_3_SPEED": "Simulate a 3-speed transmission.",
    "OVERDRIVE": "Extra overdrive gear for automatic transmission.",
    "SEMI_AUTOMATIC": "Semi-auto shifting simulation.",
    "MODE1_SHIFTING": "Use Mode 1 switch to shift between 2 speeds.",
    "TRANSMISSION_NEUTRAL": "Allow putting transmission in neutral.",
    "DOUBLE_CLUTCH": "Old-school double-clutch shifting with rev matching.",
    "HIGH_SLIPPINGPOINT": "Clutch grabs at higher RPM. Not for heavy trucks.",
    "QUICRUN_FUSION": "Hobbywing Quicrun Fusion ESC linearity fix.",
    "QUICRUN_16BL30": "Hobbywing 16BL30 ESC fix (experimental).",
    "ESC_DIR": "Reverse the motor spinning direction.",
    "HYDROSTATIC_MODE": "ESC can go opposite direction after braking (hydrostatic drive).",
    "RZ7886_DRIVER_MODE": "Using RZ7886 motor driver IC instead of a standard ESC.",
    "BATTERY_PROTECTION": "Auto-disable ESC when battery voltage drops too low.",
    "GT_POWER_STOCK": "GT-Power shaker motor with stock brass weight.",
    "GT_POWER_PLASTIC": "GT-Power shaker motor with 3D printed plastic weight.",
    "NEOPIXEL_ENABLED": "Enable WS2812 Neopixel LED strip on GPIO0.",
    "NEOPIXEL_ON_CH4": "Run Neopixels from the CH4 servo header (BUS mode only).",
    "NEOPIXEL_HIGHBEAM": "Neopixel bar acts as high beam lights.",
    "THIRD_BRAKELIGHT": "Pin 32 = 3rd brake light (otherwise it's trailer detect).",
    "ROTATINGBEACON_ON_B1": "Rotating beacon effect on beacon 1 output.",
    "INDICATOR_TOGGLING_MODE": "Indicators toggle on/off per press (for loaders).",
    "DEBUG": "Print EEPROM values to serial monitor for debugging.",
    "CHANNEL_DEBUG": "Print input channel values to serial monitor.",
    "ESC_DEBUG": "Print ESC values to serial monitor.",
    "AUTO_TRANS_DEBUG": "Print automatic transmission debug info.",
    "MANUAL_TRANS_DEBUG": "Print manual transmission debug info.",
    "TRACKED_DEBUG": "Print tracked vehicle debug info.",
    "SERVO_DEBUG": "Servo calibration debug mode.",
    "ESPNOW_DEBUG": "Print ESP-NOW wireless debug messages.",
    "ERASE_EEPROM_ON_BOOT": "Wipe all saved settings on next boot. Disable after use!",
    "ENABLE_WIRELESS": "Enable ESP-Now for wireless trailer or WiFi web config.",
    "IBUS_COMMUNICATION": "Use iBUS receiver (FlySky). Supports 13 channels.",
    "SUMD_COMMUNICATION": "Use SUMD receiver (Graupner). 12 channels.",
    "PPM_COMMUNICATION": "Use PPM receiver. 8 channels max.",
    "EMBEDDED_SBUS": "Use built-in SBUS code instead of external library.",
    "EXPONENTIAL_THROTTLE": "Smooth throttle curve for crawlers. Less twitchy.",
    "EXPONENTIAL_STEERING": "Smooth steering near center for precision.",
    "CHANNEL_AVERAGING": "Average channel readings (experimental, for noisy receivers).",
    "AUTO_LIGHTS": "Lights turn on/off with the engine automatically.",
    "AUTO_ENGINE_ON_OFF": "Engine starts/stops with a throttle gesture + timer.",
    "AUTO_INDICATORS": "Turn signals trigger from steering angle automatically.",
    "SPI_DASHBOARD": "Enable small SPI LCD screen as a dashboard.",
    "FREVIC_DASHBOARD": "Use Frevic's alternative dashboard layout.",
    "NO_SIREN": "Disable siren sound completely.",
    "NO_INDICATOR_SOUND": "Disable the indicator tick-tock sound.",
    "WEMOS_D1_MINI_ESP32": "Running on WeMos D1 Mini ESP32 trailer controller board.",
    "USE_CSS": "Simple CSS for the config web page.",
    "MODERN_CSS": "Modern CSS with mobile device scaling.",
    "CH3_BEACON": "Rotating beacons on Servo CH3 output (BUS mode).",
    "MODE2_TRAILER_UNLOCKING": "Mode 2 button unlocks trailer 5th wheel.",
    "MODE2_WINCH": "Mode 2 button controls a winch.",
    "NO_WINCH_DELAY": "Skip winch on/off ramp for instant response.",
    "MODE2_HYDRAULIC": "Mode 2 button controls hydraulic valve.",
    "PINGON_MODE": "Wheel lift feature for Pingon excavators.",
    "TRAILER_LIGHTS_TRAILER_PRESENCE_SWITCH_DEPENDENT": "Trailer lights only on when pin 32 detect switch closed.",
    # ── Bool vars ──
    "noCabLights": "Skip cab lights in the light sequence.",
    "noFogLights": "Skip fog lights in the light sequence.",
    "xenonLights": "Flash headlights when turning on (xenon style).",
    "flickeringWileCranking": "Lights flicker while engine is cranking.",
    "ledIndicators": "Turn signals switch hard on/off (no fade).",
    "swap_L_R_indicators": "Swap which side is left and right indicator.",
    "indicatorsAsSidemarkers": "Turn signals double as US-style side markers.",
    "separateFullBeam": "High beam uses its own separate output pin.",
    "flashingBlueLight": "Emergency light style. True=flash, False=rotate.",
    "hazardsWhile5thWheelUnlocked": "Hazard lights on when 5th wheel is unlocked.",
    "boomDownwardsHydraulic": "Play hydraulic load sound when boom goes down.",
    "reverseBoomSoundDirection": "Flip boom sound direction (if hoses can't be swapped).",
    "sbusInverted": "SBUS signal polarity. True = non-inverted (standard).",
    "defaultUseTrailer1": "Enable wireless communication to Trailer 1.",
    "defaultUseTrailer2": "Enable wireless communication to Trailer 2.",
    "defaultUseTrailer3": "Enable wireless communication to Trailer 3.",
    # ── Remote Control Profiles ──
    "FLYSKY_FS_I6X": "FlySky FS-i6X transmitter. Only enable one remote profile!",
    "FLYSKY_FS_I6S": "FlySky FS-i6S transmitter. Only enable one remote profile!",
    "FLYSKY_FS_I6S_LOADER": "FlySky FS-i6S for loader vehicles.",
    "FLYSKY_FS_I6S_DOZER": "FlySky FS-i6S for dozer vehicles.",
    "FLYSKY_FS_I6S_EXCAVATOR": "FlySky FS-i6S for excavator vehicles.",
    "FLYSKY_FS_I6S_EXCAVATOR_TEST": "FlySky FS-i6S excavator test/debug mode.",
    "FLYSKY_GT5": "FlySky GT5 or Reely GT6 EVO transmitter.",
    "FRSKY_TANDEM_EXCAVATOR": "FrSky Tandem XE for hydraulic excavators.",
    "FRSKY_TANDEM_HARMONY_LOADER": "FrSky Tandem XE for loaders.",
    "FRSKY_TANDEM_CRANE": "FrSky Tandem XE for crane vehicles.",
    "RGT_EX86100": "MT-305 remote (shipped with RGT EX86100 crawler).",
    "GRAUPNER_MZ_12": "Graupner MZ-12 PRO transmitter.",
    "MICRO_RC": "DIY Micro RC car-style controller.",
    "MICRO_RC_STICK": "DIY Micro RC stick-style controller.",
    "PROTOTYPE_36": "36-pin prototype board. Don't enable unless you have one!",
    "SBUS_COMMUNICATION": "Use SBUS receiver protocol (FrSky etc).",
    "EMBEDDED_SBUS": "Use built-in SBUS code instead of external library.",
    "IBUS_COMMUNICATION": "Use iBUS receiver protocol (FlySky). Supports 13 channels.",
    "SUMD_COMMUNICATION": "Use SUMD receiver protocol (Graupner). 12 channels.",
    "PPM_COMMUNICATION": "Use PPM receiver protocol. Max 8 channels.",
    "EXPONENTIAL_THROTTLE": "Smooth throttle curve near center. Great for crawlers.",
    "EXPONENTIAL_STEERING": "Smooth steering near center for more precision.",
    "CHANNEL_AVERAGING": "Average readings to reduce noise. Experimental.",
    "AUTO_LIGHTS": "Lights auto on with engine, off when engine stops.",
    "AUTO_ENGINE_ON_OFF": "Engine auto start/stop with throttle gesture + timer.",
    "AUTO_INDICATORS": "Turn signals auto-trigger based on steering angle.",
    # ── Channel Mapping ──
    "STEERING": "Which receiver channel controls steering. Usually 1.",
    "GEARBOX": "Which receiver channel controls the gearbox. Usually 6.",
    "THROTTLE": "Which receiver channel controls throttle. Usually 3.",
    "HORN": "Which receiver channel triggers the horn. Usually 5.",
    "FUNCTION_R": "Which receiver channel for right function. Usually 2.",
    "FUNCTION_L": "Which receiver channel for left function. Usually 4.",
    "POT2": "Which receiver channel for pot 2. Usually 8.",
    "MODE1": "Which receiver channel for Mode 1 switch. Usually 7.",
    "MODE2": "Which receiver channel for Mode 2 switch. Usually 9.",
    "MOMENTARY1": "Channel for momentary button. NONE to disable.",
    "HAZARDS": "Channel for hazard lights. NONE to disable.",
    "INDICATOR_LEFT": "Channel for left indicator. NONE to disable.",
    "INDICATOR_RIGHT": "Channel for right indicator. NONE to disable.",
    "CH_14": "Channel 14 number mapping.",
    "CH_15": "Channel 15 number mapping.",
    "CH_16": "Channel 16 number mapping.",
    # ── Servo Configs ──
    "SERVOS_DEFAULT": "Standard servo configuration. Only enable one config!",
    "SERVOS_ACTROS": "Mercedes Actros servo layout.",
    "SERVOS_C34": "C34 vehicle servo layout.",
    "SERVOS_CRANE": "Crane servo layout with boom control.",
    "SERVOS_EXCAVATOR": "Excavator servo layout.",
    "SERVOS_HYDRAULIC_EXCAVATOR": "Hydraulic excavator with proportional controls.",
    "SERVOS_KING_HAULER": "Tamiya King Hauler servo layout.",
    "SERVOS_LANDY_DOUBLE_EAGLE": "Land Rover Double Eagle servo layout.",
    "SERVOS_LANDY_MN_MODEL": "Land Rover MN Model servo layout.",
    "SERVOS_MECCANO_DUMPER": "Meccano dump truck servo layout.",
    "SERVOS_OPEN_RC_TRACTOR": "Open RC Project tractor servo layout.",
    "SERVOS_RACING_TRUCK": "Racing truck servo layout.",
    "SERVOS_RGT_EX86100": "RGT EX86100 crawler servo layout.",
    "SERVOS_URAL": "Ural truck servo layout.",
    # ── Dashboard ──
    "SPI_DASHBOARD": "Enable SPI LCD dashboard display on the board.",
    "FREVIC_DASHBOARD": "Use Frevic's alternative dashboard gauge layout.",
    # ── Sound Toggles ──
    "NO_SIREN": "Completely disable the siren sound.",
    "NO_INDICATOR_SOUND": "Disable the indicator tick-tock click sound.",
    # ── CSS / Web ──
    "USE_CSS": "Simple CSS for the built-in config web page.",
    "MODERN_CSS": "Modern CSS with mobile-friendly scaling.",
    "WEMOS_D1_MINI_ESP32": "Board is WeMos D1 Mini ESP32 (trailer controller).",
    "CORE_DEBUG": "ESP32 core debug output. Don't enable unless debugging!",
    "ERASE_EEPROM_ON_BOOT": "Wipe ALL saved settings next boot. Disable after!",
    "ENABLE_WIRELESS": "Turn on ESP-Now for wireless trailer or WiFi web config.",
    # ── Bool var names ──
    "noCabLights": "Skip cab lights. Turn off if your model has no cab lights.",
    "noFogLights": "Skip fog lights. Turn off if your model has no fog lights.",
    "xenonLights": "Brief headlight flash effect when turning on.",
    "flickeringWileCranking": "Lights dim and flicker during engine crank.",
    "ledIndicators": "Turn signals snap on/off instead of fading smoothly.",
    "swap_L_R_indicators": "Flip which output is left vs right indicator.",
    "indicatorsAsSidemarkers": "Indicators stay on dimly as US-style side markers.",
    "separateFullBeam": "High beam uses the roof light output pin.",
    "flashingBlueLight": "True = double flash. False = rotating beacon style.",
    "hazardsWhile5thWheelUnlocked": "Flash hazards when the 5th wheel is unlocked.",
    "boomDownwardsHydraulic": "Hydraulic sound plays when boom goes down too.",
    "reverseBoomSoundDirection": "Swap boom sound direction if hoses are reversed.",
    "sbusInverted": "SBUS polarity. True = non-inverted signal (standard).",
}


def friendly_name(raw_name):
    """Return a human-readable label for a C++ variable name."""
    return FRIENDLY_NAMES.get(raw_name, raw_name)


def extract_description(lines, i):
    inline = ""
    line = lines[i]
    if "//" in line:
        inline = clean_comment(line.split("//", 1)[1])

    if inline and "choose the" not in inline.lower():
        return inline

    prev = []
    for j in range(max(0, i - 3), i):
        s = lines[j].strip()
        if not s.startswith("//"):
            continue
        c = clean_comment(s)
        if not c:
            continue
        if "choose the" in c.lower():
            continue
        prev.append(c)

    if prev:
        return " ".join(prev[-2:])
    return ""


def parse_items(rel_path):
    path = os.path.join(SRC, rel_path)
    if not os.path.exists(path):
        return []

    text = read_text(path)
    lines = text.splitlines()
    items = []
    seen = set()

    for i, raw in enumerate(lines):
        m = re.match(r"^(\s*)(//+\s*)?#define\s+(\w+)(?:\s+(.+))?$", raw)
        if m:
            name = m.group(3)
            value = m.group(4).strip() if m.group(4) else None
            commented = bool(m.group(2))
            desc = extract_description(lines, i)

            if value and "//" in value:
                value = value.split("//", 1)[0].strip()

            # Detect natural-language descriptions masquerading as values
            # Real #define values are: numbers, ALL_CAPS identifiers, true/false
            if value and not re.match(r'^-?[\d.]+$', value) and not re.match(r'^[A-Z_][A-Z_0-9]*$', value) and value.lower() not in ('true', 'false'):
                if not desc:
                    desc = value
                value = None

            if name.endswith("_H") or name.endswith("_h"):
                continue
            if name in ("Arduino_h", "ARDUINO", "WiFi_h", "IRAM_ATTR"):
                continue
            if should_skip_entry(lines, i, name):
                continue
            if name in seen:
                continue

            seen.add(name)
            items.append(
                {
                    "kind": "define_val" if value else "define_flag",
                    "name": name,
                    "value": value,
                    "enabled": not commented,
                    "description": desc,
                }
            )
            continue

        m = re.match(
          r"^\s*(?:(?:volatile|const|static)\s+)*(uint\d+_t|int\d+_t|uint|int|bool|boolean|String|float|double|long|wifi_power_t)\s+(\w+)\s*=\s*(.+?)\s*[,;]",
            raw,
        )
        if m:
            vtype = m.group(1)
            desc = extract_description(lines, i)

            # Parse ALL variables on this comma-separated declaration line
            decl_part = raw.split("//")[0]  # strip trailing comment
            # Match each "name = value" pair
            for vm in re.finditer(r"(\w+)\s*=\s*([^,;]+)", decl_part):
                vname = vm.group(1)
                vval = vm.group(2).strip()
                # Skip the type keyword itself
                if vname == vtype or vname in ("volatile", "const", "static"):
                    continue

                if should_skip_entry(lines, i, vname):
                    continue
                if vname in seen:
                    continue

                seen.add(vname)
                items.append(
                    {
                        "kind": "text_var",
                        "vtype": vtype,
                        "name": vname,
                        "value": vval,
                        "description": desc,
                    }
                )

    return items


# --------------- sound scanning helpers ---------------

SOUND_CATEGORY_KEYWORDS = {
    "idle":       ["idle", "idling"],
    "rev":        ["rev", "medium"],
    "start":      ["start"],
    "knock":      ["knock"],
    "jakebrake":  ["jakebrake", "jake_brake", "jake"],
    "horn":       ["horn"],
    "siren":      ["siren", "anthem", "psalm", "bond", "marseillaise", "tequila", "in_the_summer", "alphorn", "brasil"],
    "airbrake":   ["airbrake", "air_brake"],
    "parking":    ["parking"],
    "shifting":   ["shifting", "airshifting"],
    "turbo":      ["turbo", "whistle"],
    "wastegate":  ["wastegate", "blowoff"],
    "fan":        ["fan", "cooling"],
    "indicator":  ["indicator", "tick"],
    "reversing":  ["reversing", "reverse_beep"],
    "coupling":   ["coupling", "uncoupling"],
    "hydraulicpump": ["hydraulicpump", "hydraulic_pump"],
    "hydraulicflow": ["hydraulicflow", "hydraulic_flow", "hydraulicfluid"],
    "trackrattle": ["trackrattle", "track_rattle", "squeaky"],
    "bucketrattle": ["bucketrattle", "bucket"],
    "supercharger": ["supercharger", "charger"],
}

def categorize_sound_file(filename):
    """Guess a sound category from filename."""
    fn = filename.lower().replace(".h", "").replace("-", "").replace("_", "")
    # Check specific categories first (order matters: jakebrake before brake)
    for cat, keywords in SOUND_CATEGORY_KEYWORDS.items():
        for kw in keywords:
            if kw.replace("_", "") in fn:
                return cat
    # Fallback
    if "brake" in fn:
        return "airbrake"
    return "other"


_all_sounds_cache = {"mtime": 0, "data": []}

def scan_all_sounds():
    """Scan every .h in src/vehicles/sounds/ and categorize."""
    sounds_dir = os.path.join(SRC, "vehicles", "sounds")
    if not os.path.isdir(sounds_dir):
        return []
    # Simple mtime cache
    dir_mtime = os.path.getmtime(sounds_dir)
    if dir_mtime == _all_sounds_cache["mtime"] and _all_sounds_cache["data"]:
        return _all_sounds_cache["data"]
    result = []
    for fn in sorted(os.listdir(sounds_dir)):
        if not fn.endswith(".h"):
            continue
        cat = categorize_sound_file(fn)
        label = fn.replace(".h", "")
        result.append({"file": fn, "label": label, "category": cat})
    _all_sounds_cache["mtime"] = dir_mtime
    _all_sounds_cache["data"] = result
    return result


def parse_sound_header(filepath):
    """Parse a .h sound file and return sampleRate, sampleCount, and PCM samples as list of ints."""
    text = read_text(filepath)
    rate_m = re.search(r'(?:unsigned\s+int|int)\s+\w*[Ss]ample[Rr]ate\s*=\s*(\d+)', text)
    count_m = re.search(r'(?:unsigned\s+int|int)\s+\w*[Ss]ample[Cc]ount\s*=\s*(\d+)', text)
    arr_m = re.search(r'(?:signed\s+char|char)\s+\w+\[\]\s*=\s*\{([^}]+)\}', text, re.DOTALL)
    if not arr_m:
        return None
    samples_str = arr_m.group(1)
    samples = [int(x.strip()) for x in samples_str.split(",") if x.strip().lstrip("-").isdigit()]
    sr = int(rate_m.group(1)) if rate_m else 22050
    sc = int(count_m.group(1)) if count_m else len(samples)
    return {"sampleRate": sr, "sampleCount": sc, "samples": samples}


def parse_sound_choices(vehicle_rel_path):
    path = os.path.join(SRC, vehicle_rel_path)
    if not os.path.exists(path):
      return []

    lines = read_text(path).splitlines()
    groups = []
    by_key = {}
    current_title = ""
    current_key = ""

    for raw in lines:
      # Reset current_key on any "// Adjust ..." or other non-Choose section header
      if re.match(r"^\s*//\s*Adjust\s+", raw, flags=re.IGNORECASE):
        current_title = ""
        current_key = ""
        continue
      m_title = re.match(r"^\s*//\s*Choose\s+(.+)$", raw, flags=re.IGNORECASE)
      if m_title:
        title = clean_comment(m_title.group(1))
        if "ones you want" in title.lower():
          current_title = ""
          current_key = ""
          continue
        current_title = title
        current_key = section_key(title)
        if current_key and current_key not in by_key:
          group = {
            "key": current_key,
            "title": current_title,
            "description": current_title,
            "options": [],
            "selected": None,
          }
          groups.append(group)
          by_key[current_key] = group
        continue

      m_inc = re.match(r'^\s*(//\s*)?#include\s+"sounds/([^"]+)"\s*(?://\s*(.*))?$', raw)
      if not m_inc or not current_key or current_key not in by_key:
        continue

      group = by_key[current_key]
      filename = m_inc.group(2)
      # Normalize double-custom prefixes
      filename = re.sub(r'^(custom)+', 'custom', filename)
      label = clean_comment(m_inc.group(3) or "")
      if not label:
        label = filename.replace(".h", "")

      group["options"].append({"file": filename, "label": label})
      if not m_inc.group(1):
        group["selected"] = filename

    return [g for g in groups if g["options"]]


# Map sound section keys to the firmware's expected variable prefixes
SOUND_VAR_PREFIX_MAP = {
    "start_sound": "start",
    "idle_sound": "",            # idle uses no prefix: samples, sampleRate, sampleCount
    "motor_idle": "",
    "revving_sound": "rev",
    "motor_rev": "rev",
    "jake_brake": "jakeBrake",
    "knock_sound": "knock",
    "ignition_knock": "knock",
    "siren": "siren",
    "horn_sound": "horn",
    "air_brake": "brake",
    "parking_brake": "parkingBrake",
    "gear_shifting": "shifting",
    "sound1": "sound1",
    "additional_sound": "sound1",
    "reversing": "reversing",
    "indicator": "indicator",
    "turn_signal": "indicator",
    "coupling": "coupling",
    "hydraulic_pump": "hydraulicPump",
    "hydraulic_fluid": "hydraulicFlow",
    "hydraulic_flow": "hydraulicFlow",
    "squeaky_track": "trackRattle",
    "track_rattle_2": "trackRattle2",
    "track_rattle": "trackRattle",
    "bucket_rattle": "bucketRattle",
    "turbo": "turbo",
    "wastegate": "wastegate",
    "blowoff": "wastegate",
    "supercharger": "charger",
    "charger": "charger",
    "fan_sound": "fan",
    "cooling_fan": "fan",
}


def get_var_prefix_for_key(section_key_str):
    """Return the firmware variable prefix for a section key, or None if unknown."""
    for pattern, prefix in SOUND_VAR_PREFIX_MAP.items():
        if pattern in section_key_str:
            return prefix
    return None


def fix_sound_file_variables(filepath, var_prefix):
    """Rewrite variable names in a sound .h file to match firmware expectations.
    var_prefix="" means idle (bare names), otherwise e.g. "start", "trackRattle2"."""
    if not os.path.isfile(filepath):
        return
    text = read_text(filepath)
    m_arr = re.search(r"const\s+signed\s+char\s+(\w+)\s*\[\]", text)
    if not m_arr:
        return
    old_name = m_arr.group(1)
    if var_prefix == "":
        new_arr, new_rate, new_count = "samples", "sampleRate", "sampleCount"
    else:
        new_arr = var_prefix + "Samples"
        new_rate = var_prefix + "SampleRate"
        new_count = var_prefix + "SampleCount"
    # Check if already correct
    if old_name == new_arr:
        return
    text = text.replace(old_name + "[]", new_arr + "[]")
    text = text.replace(old_name + " []", new_arr + "[]")
    text = re.sub(r"\b" + re.escape(old_name) + r"_sampleRate\b", new_rate, text)
    text = re.sub(r"\b" + re.escape(old_name) + r"SampleRate\b", new_rate, text)
    text = re.sub(r"\b" + re.escape(old_name) + r"_sampleCount\b", new_count, text)
    text = re.sub(r"\b" + re.escape(old_name) + r"SampleCount\b", new_count, text)
    if new_count not in text:
        m_body = re.search(r"const\s+signed\s+char\s+\w+\[\]\s*=\s*\{([^}]+)\}", text, re.DOTALL)
        if m_body:
            count = len([x for x in m_body.group(1).split(",") if x.strip()])
            count_line = "const unsigned int %s = %d;\n" % (new_count, count)
            text = re.sub(r"(const\s+signed\s+char\s+)", count_line + r"\1", text, count=1)
    write_text(filepath, text)


# Map of pattern (in section key) → dummy .h file to use when "None" is selected.
# These provide silent placeholder variables so the firmware still compiles.
DUMMY_FILES = {
    'start':          'StartDummy.h',
    'idle':           'idleDummy.h',
    'rev':            'RevDummy.h',
    'knock':          'DieselKnockDummy.h',
    'jake_brake':     'JakeBrakeDummy.h',
    'turbo':          'TurboDummy.h',
    'supercharger':   'SuperchargerDummy.h',
    'wastegate':      'WastegateDummy.h',
    'fan':            'FanDummy.h',
    'horn':           'HornDummy.h',
    'siren':          'sirenDummy.h',
    'air_brake':      'AirBrakeDummy.h',
    'parking_brake':  'ParkingBrakeDummy.h',
    'air_shifting':   'AirShiftingDummy.h',
    'shifting':       'AirShiftingDummy.h',
    'sound1':         'Sound1Dummy.h',
    'reversing':      'ReversingSoundDummy.h',
    'indicator':      'IndicatorDummy.h',
    'coupling':       'CouplingDummy.h',
    'hydraulic_pump': 'HydraulicPumpDummy.h',
    'hydraulic_flow': 'HydraulicFlowDummy.h',
    'hydraulic_fluid':'HydraulicFlowDummy.h',
    'squeaky_track':  'TrackRattleDummy.h',
    'track_rattle_2': 'TrackRattle2Dummy.h',
    'bucket_rattle':  'BucketRattleDummy.h',
}

def _dummy_for_section(key):
    """Return the dummy filename for a section key, or None."""
    for pattern, dummy in DUMMY_FILES.items():
        if pattern in key:
            return dummy
    return None


def validate_and_fix_vehicle(vehicle_file):
    """Scan a vehicle .h file and ensure every sound section has an active include.
    If a section has zero uncommented #include "sounds/..." lines, inject the
    appropriate dummy so the build never fails.  Also ensures 1_Vehicle.h has
    the vehicle uncommented.  Returns a list of fixes applied (empty = all ok)."""
    fixes = []

    # 1) Ensure 1_Vehicle.h has this vehicle active
    veh1 = os.path.join(SRC, "1_Vehicle.h")
    if os.path.isfile(veh1):
        cur = get_current_vehicle()
        if cur != vehicle_file:
            apply_vehicle(vehicle_file)
            fixes.append("Activated %s in 1_Vehicle.h" % vehicle_file)

    # 2) Scan the vehicle file for broken sections
    vpath = os.path.join(SRC, "vehicles", vehicle_file)
    if not os.path.isfile(vpath):
        return fixes

    lines = read_text(vpath).splitlines()
    current_key = ""
    section_start = -1
    section_has_active = False
    # Track section spans so we can patch
    sections = []  # list of (key, start_line, end_line, has_active_include)

    def _flush(end_at=None):
        nonlocal current_key, section_start, section_has_active
        if current_key and section_start >= 0:
            sections.append((current_key, section_start, end_at if end_at is not None else len(lines) - 1, section_has_active))
        current_key = ""
        section_start = -1
        section_has_active = False

    for i, raw in enumerate(lines):
        # Match "Choose" or "Adjust" section headers
        m_choose = re.match(r"^\s*//\s*Choose\s+(.+)$", raw, flags=re.IGNORECASE)
        m_adjust = re.match(r"^\s*//\s*Adjust\s+(.+)$", raw, flags=re.IGNORECASE)
        if m_choose or m_adjust:
            _flush(i - 1)
            title = clean_comment((m_choose or m_adjust).group(1))
            sk = section_key(title)
            # Skip non-sound sections
            if "light" in sk or "blue_light" in sk or "excavator_specific" in sk:
                continue
            current_key = sk
            section_start = i
            section_has_active = False
            continue
        # Check for active include in current section
        if current_key:
            m_inc = re.match(r'^\s*#include\s+"sounds/([^"]+)"', raw)
            if m_inc:
                section_has_active = True

    _flush()

    # Collect all sound files already actively included (to avoid redefinition)
    active_files = set()
    for i, raw in enumerate(lines):
        m = re.match(r'^\s*#include\s+"sounds/([^"]+)"', raw)
        if m:
            active_files.add(m.group(1))

    # 3) For each section that has no active include, inject a dummy
    inserted = 0
    for sk, s_start, s_end, has_active in sections:
        if has_active:
            continue
        dummy = _dummy_for_section(sk)
        if not dummy:
            continue
        # Don't inject a file that's already included elsewhere (would cause redefinition)
        if dummy in active_files:
            continue
        # Find the best insert point: after the last #include line in section,
        # but BEFORE any #endif so we stay inside #ifdef blocks
        insert_at = s_start + 1
        endif_line = None
        for j in range(s_start, min(s_end + 1, len(lines) + inserted)):
            if re.match(r'^\s*(//\s*)?#include\s+"sounds/', lines[j]):
                insert_at = j + 1
            if re.match(r'^\s*#endif', lines[j]):
                endif_line = j
        # If there's an #endif in this section, insert before it (inside the #ifdef block)
        if endif_line is not None and insert_at > endif_line:
            insert_at = endif_line
        new_line = '#include "sounds/%s" // auto-fix: silent placeholder' % dummy
        lines.insert(insert_at + inserted, new_line)
        active_files.add(dummy)
        inserted += 1
        fixes.append("Added %s for section '%s'" % (dummy, sk))

    if inserted > 0:
        write_text(vpath, "\n".join(lines) + "\n")

    return fixes

def apply_sound_choices(vehicle_rel_path, choices):
    if not choices:
      return

    # Filter out empty selections and fix double-custom prefixes
    cleaned = {}
    for k, v in choices.items():
      if not v or not v.strip():
        continue
      # Fix customcustom... → custom... (a bug created these)
      fixed = re.sub(r'^(custom)+', 'custom', v.strip())
      cleaned[k] = fixed
    choices = cleaned

    path = os.path.join(SRC, vehicle_rel_path)
    lines = read_text(path).splitlines()
    out = []
    current_key = ""
    written_keys = set()   # track which keys we actually wrote the selected file for
    section_files = {}     # key -> set of files already in that section
    # Track lines-per-section so we can deduplicate afterwards
    section_line_indices = {}  # key -> list of (out_index, filename, is_uncommented)

    def _maybe_inject():
        """Inject a new #include if the selected file wasn't already in this section."""
        nonlocal current_key
        if current_key and current_key in choices and current_key not in written_keys:
          sel = choices[current_key]
          if sel and sel != '__none__' and sel not in section_files.get(current_key, set()):
            # Check if the last line(s) in out are #endif — insert before them
            insert_idx = len(out)
            # Walk backwards past #endif / blank lines to stay inside #ifdef blocks
            while insert_idx > 0 and re.match(r'^\s*(#endif|$)', out[insert_idx - 1]):
                insert_idx -= 1
            if insert_idx < 1:
                insert_idx = len(out)
            new_line = '#include "sounds/%s" // %s (custom)' % (sel, sel.replace('.h', ''))
            out.insert(insert_idx, new_line)
            section_line_indices.setdefault(current_key, []).append((insert_idx, sel, True))
            # Adjust indices of entries that shifted
            for k2, entries2 in section_line_indices.items():
                section_line_indices[k2] = [
                    (idx + 1 if idx >= insert_idx and not (k2 == current_key and fn == sel) else idx, fn, active)
                    for idx, fn, active in entries2
                ]
            written_keys.add(current_key)

    for raw in lines:
      # Reset current_key on any "// Adjust ..." section header (not a Choose section)
      if re.match(r"^\s*//\s*Adjust\s+", raw, flags=re.IGNORECASE):
        _maybe_inject()
        current_key = ""
        out.append(raw)
        continue
      m_title = re.match(r"^\s*//\s*Choose\s+(.+)$", raw, flags=re.IGNORECASE)
      if m_title:
        _maybe_inject()
        title = clean_comment(m_title.group(1))
        if "ones you want" in title.lower():
          current_key = ""
        else:
          current_key = section_key(title)
        out.append(raw)
        continue

      m_inc = re.match(r'^(\s*)(//\s*)?#include\s+"sounds/([^"]+)"(\s*//.*)?$', raw)
      if m_inc and current_key in choices:
        indent = m_inc.group(1) or ""
        filename = m_inc.group(3)
        # Normalize double-custom prefixes in existing includes
        norm_filename = re.sub(r'^(custom)+', 'custom', filename)
        tail = m_inc.group(4) or ""
        selected_file = choices[current_key]
        # "None" selection: comment out all includes EXCEPT dummy placeholders
        if selected_file == '__none__':
          dummy = _dummy_for_section(current_key)
          is_dummy = (dummy and filename == dummy)
          idx = len(out)
          if is_dummy:
            # Keep the dummy uncommented so firmware variables are defined
            new_line = indent + '#include "sounds/%s"%s' % (filename, tail)
            out.append(new_line)
            section_line_indices.setdefault(current_key, []).append((idx, filename, True))
          else:
            new_line = indent + '// #include "sounds/%s"%s' % (filename, tail)
            out.append(new_line)
            section_line_indices.setdefault(current_key, []).append((idx, filename, False))
          written_keys.add(current_key)
          continue
        # Track files in this section (both original and normalized)
        section_files.setdefault(current_key, set()).add(filename)
        section_files[current_key].add(norm_filename)
        # If this line is a double-custom variant of the selection, rewrite it
        if norm_filename == selected_file and filename != selected_file:
          idx = len(out)
          out.append(indent + '#include "sounds/%s"%s' % (selected_file, tail))
          section_line_indices.setdefault(current_key, []).append((idx, selected_file, True))
          written_keys.add(current_key)
          continue
        is_selected = (filename == selected_file)
        if is_selected:
          written_keys.add(current_key)
        idx = len(out)
        new_line = indent
        if not is_selected:
          new_line += "// "
        new_line += '#include "sounds/%s"%s' % (filename, tail)
        out.append(new_line)
        section_line_indices.setdefault(current_key, []).append((idx, filename, is_selected))
        continue

      out.append(raw)

    # Handle last section
    _maybe_inject()

    # ── Inject dummy if "None" was selected but no dummy existed in the section ──
    for key in list(section_line_indices.keys()):
        sel = choices.get(key, '')
        if sel != '__none__':
            continue
        dummy = _dummy_for_section(key)
        if not dummy:
            continue
        entries = section_line_indices[key]
        # Check if a dummy is already present (uncommented) in this section
        has_active_dummy = any(fn == dummy and active for _, fn, active in entries)
        if has_active_dummy:
            continue
        # No dummy in section — inject one after the last line
        last_idx = max(idx for idx, fn, active in entries)
        out.insert(last_idx + 1, '#include "sounds/%s" // silent placeholder (disabled)' % dummy)
        for k2, entries2 in section_line_indices.items():
            section_line_indices[k2] = [
                (idx + 1 if idx > last_idx else idx, fn, active)
                for idx, fn, active in entries2
            ]

    # ── Deduplicate: ensure only ONE uncommented include per section ──
    for key, entries in section_line_indices.items():
        uncommented = [(idx, fn) for idx, fn, active in entries if active]
        if len(uncommented) > 1:
            # Keep the FIRST uncommented line, comment out all others
            for idx, fn in uncommented[1:]:
                line = out[idx]
                if not line.lstrip().startswith("//"):
                    out[idx] = "// " + line

    write_text(path, "\n".join(out) + "\n")

    # Fix variable names in all selected sound files to match firmware expectations
    sounds_dir = os.path.join(SRC, "vehicles", "sounds")
    for key, sel_file in choices.items():
        vp = get_var_prefix_for_key(key)
        if vp is not None:
            fpath = os.path.join(sounds_dir, sel_file)
            # If this file is shared with another section, COPY it first
            other_keys_using_file = [k for k, f in choices.items() if f == sel_file and k != key]
            if other_keys_using_file:
                # This file is used by multiple sections — skip fixing here,
                # it will be handled by the section that "owns" the filename
                continue
            fix_sound_file_variables(fpath, vp)


def is_advanced_sound_tuning(name):
    n = (name or "").lower()
    tokens = (
      "volumepercentage",
      "switchpoint",
      "endpoint",
      "interval",
      "startpoint",
      "minrpm",
      "max_rpm",
      "ramp",
      "acceleration",
      "deceleration",
      "idlevolume",
    )
    return any(t in n for t in tokens)


def get_vehicle_list():
    vdir = os.path.join(SRC, "vehicles")
    if not os.path.isdir(vdir):
        return []
    return sorted(f for f in os.listdir(vdir) if f.endswith(".h") and not f.startswith("."))


def get_current_vehicle():
    path = os.path.join(SRC, "1_Vehicle.h")
    if not os.path.exists(path):
        return None
    m = re.search(r'^\s*#include\s+"vehicles/([^"]+)"', read_text(path), re.MULTILINE)
    return m.group(1) if m else None


def apply_changes(rel_path, changes):
    path = os.path.join(SRC, rel_path)
    text = read_text(path)

    for name, val in changes.items():
        if isinstance(val, bool):
            if val:
                text = re.sub(
                    r"^(\s*)//+\s*(#define\s+" + re.escape(name) + r"\s*)$",
                    r"\1\2",
                    text,
                    flags=re.MULTILINE,
                )
            else:
                text = re.sub(
                    r"^(\s*)(#define\s+" + re.escape(name) + r"\s*)$",
                    r"\1// \2",
                    text,
                    flags=re.MULTILINE,
                )
        else:
            text = re.sub(
                r"^(\s*#define\s+" + re.escape(name) + r"\s+).+$",
                lambda m, nv=val: m.group(1) + nv,
                text,
                flags=re.MULTILINE,
            )
            # Replace a specific "name = value" in a (possibly comma-separated) declaration
            text = re.sub(
                r"(" + re.escape(name) + r"\s*=\s*)([^,;]+)",
                lambda m, nv=val: m.group(1) + nv,
                text,
            )

    write_text(path, text)


def apply_vehicle(vehicle_file):
    path = os.path.join(SRC, "1_Vehicle.h")
    lines = read_text(path).splitlines()
    inc_re = re.compile(r'^(\s*)(//\s*)?(#include\s+"vehicles/)([^"]+)(".*)')
    out = []
    found = False
    for line in lines:
        m = inc_re.match(line)
        if m:
            indent, _comment, prefix, filename, suffix = m.groups()
            if filename == vehicle_file:
                # Uncomment the target vehicle
                out.append(indent + prefix + filename + suffix)
                found = True
            else:
                # Comment out everything else
                if not _comment:
                    out.append(indent + "// " + prefix + filename + suffix)
                else:
                    out.append(line)  # already commented
        else:
            out.append(line)
    # If the vehicle wasn't listed yet (e.g. a user-created copy), append it
    if not found:
        veh_path = os.path.join(SRC, "vehicles", vehicle_file)
        if os.path.isfile(veh_path):
            out.append('#include "vehicles/%s"' % vehicle_file)
    write_text(path, "\n".join(out) + "\n")


def list_serial_ports():
    ports = []

    try:
        from serial.tools import list_ports  # type: ignore

        ports = [p.device for p in list_ports.comports()]
    except Exception:
        ports = []

    if not ports:
        try:
            cmd = ["pio", "device", "list", "--json-output"]
            proc = subprocess.run(
                cmd,
                cwd=ROOT,
                capture_output=True,
                text=True,
                shell=(os.name == "nt"),
                check=False,
            )
            if proc.returncode == 0 and proc.stdout.strip():
                data = json.loads(proc.stdout)
                for item in data:
                    port = item.get("port")
                    if port:
                        ports.append(port)
        except Exception:
            pass

    cleaned = sorted(set(ports), key=lambda p: (len(p), p))
    return cleaned


def find_arduino_cli():
    """Return path to arduino-cli, or None if not found."""
    import shutil
    found = shutil.which("arduino-cli")
    if found:
        return found
    # Arduino IDE 2.x bundles arduino-cli
    home = os.environ.get("USERPROFILE") or os.path.expanduser("~")
    if os.name == "nt":
        candidates = [
            os.path.join(os.environ.get("LOCALAPPDATA", ""), "Programs", "Arduino IDE", "resources", "app", "lib", "backend", "resources", "arduino-cli.exe"),
            r"C:\Program Files\Arduino IDE\resources\app\lib\backend\resources\arduino-cli.exe",
            r"C:\Program Files (x86)\Arduino IDE\resources\app\lib\backend\resources\arduino-cli.exe",
        ]
    elif sys.platform == "darwin":
        candidates = [
            "/Applications/Arduino IDE.app/Contents/Resources/app/lib/backend/resources/arduino-cli",
            os.path.expanduser("~/Applications/Arduino IDE.app/Contents/Resources/app/lib/backend/resources/arduino-cli"),
            "/opt/homebrew/bin/arduino-cli",
            "/usr/local/bin/arduino-cli",
        ]
    else:
        candidates = [
            "/usr/local/bin/arduino-cli",
            "/usr/bin/arduino-cli",
            os.path.join(home, "bin", "arduino-cli"),
        ]
    for c in candidates:
        if c and os.path.isfile(c):
            return c
    return None


REQUIRED_CORE = "esp32:esp32"
REQUIRED_CORE_VERSION = "2.0.17"
ESP32_BOARD_URL = "https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json"

_core_ready = False  # cached after first successful check


def ensure_esp32_core(cli, chunk_fn=None):
    """Make sure esp32:esp32@2.0.17 is installed. Installs automatically if needed.
    chunk_fn is an optional callback for streaming status text to the browser."""
    global _core_ready
    if _core_ready:
        return True

    def msg(text):
        if chunk_fn:
            chunk_fn(text + "\n")

    # Add ESP32 board manager URL if missing
    try:
        proc = subprocess.run(
            [cli, "config", "dump", "--format", "json"],
            capture_output=True, text=True, shell=(os.name == "nt"), check=False
        )
        if proc.returncode == 0:
            cfg = json.loads(proc.stdout)
            urls = cfg.get("board_manager", {}).get("additional_urls", [])
            if ESP32_BOARD_URL not in urls:
                msg("Adding ESP32 board index URL...")
                subprocess.run(
                    [cli, "config", "add", "board_manager.additional_urls", ESP32_BOARD_URL],
                    capture_output=True, text=True, shell=(os.name == "nt"), check=False
                )
    except Exception:
        pass

    # Check installed core version
    try:
        proc = subprocess.run(
            [cli, "core", "list", "--format", "json"],
            capture_output=True, text=True, shell=(os.name == "nt"), check=False
        )
        if proc.returncode == 0:
            cores = json.loads(proc.stdout)
            for c in cores:
                cid = c.get("id", "")
                ver = c.get("installed_version", "") or c.get("installed", "")
                if cid == REQUIRED_CORE and ver == REQUIRED_CORE_VERSION:
                    msg("ESP32 core v%s OK." % REQUIRED_CORE_VERSION)
                    _core_ready = True
                    return True
    except Exception:
        pass

    # Update index and install correct version
    msg("Installing ESP32 core v%s (this may take a few minutes on first run)..." % REQUIRED_CORE_VERSION)
    subprocess.run(
        [cli, "core", "update-index"],
        capture_output=True, text=True, shell=(os.name == "nt"), check=False
    )

    proc = subprocess.run(
        [cli, "core", "install", "%s@%s" % (REQUIRED_CORE, REQUIRED_CORE_VERSION)],
        capture_output=True, text=True, shell=(os.name == "nt"), check=False
    )
    if proc.returncode == 0:
        msg("ESP32 core v%s installed successfully." % REQUIRED_CORE_VERSION)
        _core_ready = True
        return True
    else:
        msg("ERROR: Failed to install ESP32 core: " + (proc.stderr or proc.stdout or "unknown error"))
        return False


def get_library_paths():
    """Return list of --library flags for arduino-cli pointing to bundled libs."""
    libdeps = os.path.join(ROOT, ".pio", "libdeps", "esp32dev")
    libs = []
    if os.path.isdir(libdeps):
        for entry in os.listdir(libdeps):
            full = os.path.join(libdeps, entry)
            if os.path.isdir(full):
                libs.append(full)
    return libs


def get_build_flags():
    """Return the TFT_eSPI build flags from platformio.ini as a single string."""
    return " ".join([
        "-DUSER_SETUP_LOADED=1",
        "-DST7735_DRIVER=1",
        "-DTFT_WIDTH=80",
        "-DTFT_HEIGHT=160",
        "-DST7735_REDTAB160x80=1",
        "-DTFT_RGB_ORDER=TFT_BGR",
        "-DTFT_MISO=-1",
        "-DTFT_MOSI=23",
        "-DTFT_SCLK=18",
        "-DTFT_CS=-1",
        "-DTFT_DC=19",
        "-DTFT_RST=21",
        "-DLOAD_GLCD=1",
        "-DLOAD_FONT2=1",
        "-DLOAD_FONT4=1",
        "-DSPI_FREQUENCY=27000000",
        "-DUSE_HSPI_PORT=1",
    ])


def open_arduino_ide(sketch_path):
    """Try to open Arduino IDE with the given sketch. Returns (ok, message)."""
    if not os.path.isfile(sketch_path):
        return False, "Sketch file not found: %s" % sketch_path

    if os.name == "nt":
        candidates = [
            os.path.join(os.environ.get("LOCALAPPDATA", ""), "Programs", "Arduino IDE", "Arduino IDE.exe"),
            r"C:\Program Files\Arduino IDE\Arduino IDE.exe",
            r"C:\Program Files (x86)\Arduino IDE\Arduino IDE.exe",
            r"C:\Program Files\Arduino\arduino.exe",
            r"C:\Program Files (x86)\Arduino\arduino.exe",
        ]
        for exe in candidates:
            if exe and os.path.isfile(exe):
                subprocess.Popen([exe, sketch_path], cwd=ROOT)
                return True, "Opened Arduino IDE: %s" % exe

    elif sys.platform == "darwin":
        app_paths = [
            "/Applications/Arduino IDE.app",
            os.path.expanduser("~/Applications/Arduino IDE.app"),
            "/Applications/Arduino.app",
        ]
        for app in app_paths:
            if os.path.exists(app):
                subprocess.Popen(["open", "-a", app, sketch_path], cwd=ROOT)
                return True, "Opened Arduino IDE app: %s" % app

    else:
        for cmd_name in ("arduino-ide", "arduino"):
            import shutil
            exe = shutil.which(cmd_name)
            if exe:
                subprocess.Popen([exe, sketch_path], cwd=ROOT)
                return True, "Opened Arduino IDE command: %s" % exe

    return False, "Arduino IDE was not found. Install Arduino IDE 2.x and retry."


def render_section_html(rel, label, vehicles, current_vehicle, selected_vehicle=False):
    items = parse_items(rel)
    sound_choices = parse_sound_choices(rel) if rel.startswith("vehicles/") else []

    if not items and not sound_choices and not (rel == "1_Vehicle.h" and vehicles):
        return ""

    rows = []

    if rel == "1_Vehicle.h" and vehicles:
        opts = "".join(
            '<option value="%s"%s>%s</option>'
            % (esc(v), " selected" if v == current_vehicle else "", esc(v))
            for v in vehicles
        )
        rows.append(
            '<tr><td class="name">Vehicle File</td>'
            '<td><select name="__vehicle__" onchange="handleVehicleSelection(this)">%s</select></td>'
            '<td class="hint">Choose vehicle profile. Switching is live and keeps unsaved per-vehicle drafts in this session.</td></tr>'
            % opts
        )

    for si, sc in enumerate(sound_choices, 1):
        # Inject all matching sounds from the project into this dropdown
        existing_files = {o["file"] for o in sc["options"]}
        cat = categorize_sound_file(sc["key"])  # figure out what category this dropdown is
        all_sounds = scan_all_sounds()
        extras = []
        for s in all_sounds:
            if s["file"] not in existing_files and s["category"] == cat:
                extras.append({"file": s["file"], "label": s["label"]})
        all_options = sc["options"] + extras

        # Clean up the title for display
        nice_title = re.sub(r"\s*\(uncomment.*", "", sc["title"], flags=re.IGNORECASE).strip()
        nice_title = re.sub(r"\s*\(played in.*", "", nice_title, flags=re.IGNORECASE).strip()
        nice_title = re.sub(r"\s*uncomment\s+.*", "", nice_title, flags=re.IGNORECASE).strip()
        nice_title = re.sub(r"\s*comment\s+.*out.*", "", nice_title, flags=re.IGNORECASE).strip()
        nice_title = re.sub(r"\s*don.t\s+uncomment.*", "", nice_title, flags=re.IGNORECASE).strip()
        nice_title = nice_title.strip(" -,")
        nice_title = re.sub(r"^the\s+", "", nice_title, flags=re.IGNORECASE)
        nice_title = nice_title[0].upper() + nice_title[1:] if nice_title else sc["title"]
        numbered_title = "%d. %s" % (si, nice_title)

        sound_help = esc("Select active sound sample for: " + nice_title)
        opts = "".join(
            '<option value="%s"%s>%s</option>'
            % (
                esc(o["file"]),
                " selected" if o["file"] == sc.get("selected") else "",
                esc(o["label"]),
            )
            for o in all_options
        )
        if not opts:
            opts = '<option value="">-- No sounds available --</option>'
        rows.append(
            '<tr><td class="name">%s</td>'
            '<td><select name="__sound__%s" onchange="markDirty()" title="%s" data-i18n-title="tooltipSoundPicker">%s</select>'
            ' <button type="button" class="btn-cyan" style="font-size:10px;padding:1px 6px" '
            'onclick="previewSoundFromDropdown(this)" title="Preview this sound">&#9654;</button></td>'
            '<td class="hint">%s</td></tr>'
            % (esc(numbered_title), esc(sc["key"]), sound_help, opts, esc(nice_title))
        )

    for it in items:
      raw_name = it["name"]
      field_name = esc(raw_name)  # form field name must match C++ variable name
      display_name = esc(friendly_name(raw_name))  # human-readable label
      # Use our friendly description if available, otherwise fall back to source comment
      if raw_name in FRIENDLY_DESCRIPTIONS:
        full_desc = esc(FRIENDLY_DESCRIPTIONS[raw_name])
        short_desc = esc(FRIENDLY_DESCRIPTIONS[raw_name])
      else:
        full_desc = esc(it.get("description") or "Setting from source header.")
        short_desc = esc(simplify_description(it.get("description") or ""))
      row_class = ' class="adv-sound"' if rel.startswith("vehicles/") and is_advanced_sound_tuning(it["name"]) else ""

      if it["kind"] == "define_flag":
        chk = " checked" if it["enabled"] else ""
        rows.append(
          '<tr%s><td class="name">%s</td>'
          '<td><label class="sw"><input type="checkbox" name="%s"%s onchange="markDirty()" title="%s"><span class="sl"></span></label></td>'
          '<td class="hint">%s</td></tr>'
          % (row_class, display_name, field_name, chk, full_desc, short_desc)
        )

      elif it["kind"] == "define_val":
        val = esc(it["value"] or "")
        en = " checked" if it["enabled"] else ""
        slider_cfg = SLIDER_FIELDS.get(raw_name)
        is_numeric = False
        if not slider_cfg:
          try:
            float(val)
            is_numeric = True
          except (ValueError, TypeError):
            pass
        if slider_cfg:
          smin, smax, sstep, suffix = slider_cfg
          try:
            num_val = int(float(val))
          except (ValueError, TypeError):
            num_val = smin
          rows.append(
            '<tr%s><td class="name">%s</td>'
            '<td style="display:flex;align-items:center;gap:6px">'
            '<label class="sw" style="vertical-align:middle;flex-shrink:0">'
            '<input type="checkbox" name="%s__enabled"%s onchange="markDirty()" title="%s">'
            '<span class="sl"></span></label>'
            '<input type="range" name="%s" min="%d" max="%d" step="%d" value="%d" '
            'oninput="this.nextElementSibling.textContent=this.value+\'%s\';markDirty()" '
            'onchange="markDirty()" title="%s" '
            'style="flex:1;min-width:100px">'
            '<span class="slider-val">%d%s</span></td>'
            '<td class="hint">%s</td></tr>'
            % (row_class, display_name, field_name, en, full_desc,
               field_name, smin, smax, sstep, num_val,
               esc(suffix), full_desc, num_val, esc(suffix), short_desc)
          )
        elif is_numeric:
          try:
            num_val = int(float(val))
          except (ValueError, TypeError):
            num_val = 0
          auto_max = max(abs(num_val) * 3, 10) if num_val != 0 else 100
          auto_step = max(auto_max // 50, 1)
          auto_min = -auto_max if num_val < 0 else 0
          rows.append(
            '<tr%s><td class="name">%s</td>'
            '<td style="display:flex;align-items:center;gap:6px">'
            '<label class="sw" style="vertical-align:middle;flex-shrink:0">'
            '<input type="checkbox" name="%s__enabled"%s onchange="markDirty()" title="%s">'
            '<span class="sl"></span></label>'
            '<input type="range" name="%s" min="%d" max="%d" step="%d" value="%d" '
            'oninput="this.nextElementSibling.textContent=this.value;markDirty()" '
            'onchange="markDirty()" title="%s" '
            'style="flex:1;min-width:100px">'
            '<span class="slider-val">%d</span></td>'
            '<td class="hint">%s</td></tr>'
            % (row_class, display_name, field_name, en, full_desc,
               field_name, auto_min, auto_max, auto_step, num_val,
               full_desc, num_val, short_desc)
          )
        else:
          rows.append(
            '<tr%s><td class="name">%s</td>'
            '<td><label class="sw" style="vertical-align:middle">'
            '<input type="checkbox" name="%s__enabled"%s onchange="markDirty()" title="%s">'
            '<span class="sl"></span></label> '
            '<input type="text" name="%s" value="%s" onchange="markDirty()" title="%s"></td>'
            '<td class="hint">%s</td></tr>'
            % (row_class, display_name, field_name, en, full_desc, field_name, val, full_desc, short_desc)
          )

      elif it["kind"] == "text_var":
        vtype = it.get("vtype", "")
        val = esc(it["value"])
        if vtype in ("bool", "boolean"):
          chk = " checked" if it["value"].strip().lower() == "true" else ""
          rows.append(
            '<tr%s><td class="name">%s</td>'
            '<td><label class="sw"><input type="checkbox" name="%s" data-vartype="bool"%s onchange="markDirty()" title="%s"><span class="sl"></span></label></td>'
            '<td class="hint">%s</td></tr>'
            % (row_class, display_name, field_name, chk, full_desc, short_desc)
          )
        else:
          slider_cfg = SLIDER_FIELDS.get(raw_name)
          is_numeric = False
          if not slider_cfg:
            try:
              float(val)
              is_numeric = True
            except (ValueError, TypeError):
              pass
          if slider_cfg:
            smin, smax, sstep, suffix = slider_cfg
            try:
              num_val = int(float(val))
            except (ValueError, TypeError):
              num_val = smin
            rows.append(
              '<tr%s><td class="name">%s</td>'
              '<td style="display:flex;align-items:center;gap:6px">'
              '<input type="range" name="%s" min="%d" max="%d" step="%d" value="%d" '
              'oninput="this.nextElementSibling.textContent=this.value+\'%s\';markDirty()" '
              'onchange="markDirty()" title="%s" '
              'style="flex:1;min-width:100px">'
              '<span class="slider-val">%d%s</span></td>'
              '<td class="hint">%s</td></tr>'
              % (row_class, display_name, field_name, smin, smax, sstep, num_val,
                 esc(suffix), full_desc, num_val, esc(suffix), short_desc)
            )
          elif is_numeric:
            try:
              num_val = int(float(val))
            except (ValueError, TypeError):
              num_val = 0
            auto_max = max(abs(num_val) * 3, 10) if num_val != 0 else 100
            auto_step = max(auto_max // 50, 1)
            auto_min = -auto_max if num_val < 0 else 0
            rows.append(
              '<tr%s><td class="name">%s</td>'
              '<td style="display:flex;align-items:center;gap:6px">'
              '<input type="range" name="%s" min="%d" max="%d" step="%d" value="%d" '
              'oninput="this.nextElementSibling.textContent=this.value;markDirty()" '
              'onchange="markDirty()" title="%s" '
              'style="flex:1;min-width:100px">'
              '<span class="slider-val">%d</span></td>'
              '<td class="hint">%s</td></tr>'
              % (row_class, display_name, field_name, auto_min, auto_max, auto_step, num_val,
                 full_desc, num_val, short_desc)
            )
          else:
            rows.append(
              '<tr%s><td class="name">%s</td>'
              '<td><input type="text" name="%s" value="%s" onchange="markDirty()" title="%s"></td>'
              '<td class="hint">%s</td></tr>'
              % (row_class, display_name, field_name, val, full_desc, short_desc)
            )

    if not rows:
      return ""

    safe_id = re.sub(r"\W", "_", rel)
    attrs = ""
    if selected_vehicle:
      attrs = ' id="selectedVehicleDetails" data-vehicle-file="%s"' % esc(current_vehicle or "")

    return (
      '<details%s>'
      '<summary>%s <small class="file">(%s)</small></summary>'
      '<form id="form_%s" data-file="%s" onsubmit="return false">'
      '<table>%s</table>'
      '</form>'
      '</details>'
      % (attrs, esc(label), esc(rel), safe_id, esc(rel), "".join(rows))
    )


PAGE_TEMPLATE = r"""<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>TheDIYGuy999 Light Sound &amp; Speed Controller</title>
<link rel="preconnect" href="https://fonts.googleapis.com">
<link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
<link href="https://fonts.googleapis.com/css2?family=Bungee+Shade&family=Orbitron:wght@700;900&display=swap" rel="stylesheet">
<style>
:root {
  --bg: #05070a;
  --header: #081018;
  --card: #0a121a;
  --border: #164a56;
  --text: #d4f7ff;
  --dim: #7fb8c2;
  --accent: #27ffd8;
  --input: #060c11;
  --blue: #18b8ff;
  --orange: #64ffd9;
  --green: #1ce8b5;
}
* { box-sizing: border-box; margin: 0; padding: 0; }
body {
  background: radial-gradient(1200px 620px at 14% -20%, #0b2a33 0%, #04080d 48%, var(--bg) 100%);
  color: var(--text);
  font: 14px "Trebuchet MS", "Segoe UI", Tahoma, sans-serif;
  padding-bottom: 210px;
}
header {
  position: sticky;
  top: 0;
  z-index: 100;
  background: linear-gradient(180deg, #071520 0%, #050e18 60%, #030a10 100%);
  border-bottom: 2px solid var(--accent);
  box-shadow: 0 0 32px rgba(39,255,216,0.25), 0 2px 0 rgba(255,215,0,0.15);
  padding: 6px 12px 4px;
  display: flex;
  gap: 8px;
  align-items: flex-start;
  flex-wrap: wrap;
}

/* ── brand ────────────────────────────────── */
.brand {
  display: flex;
  flex-direction: column;
  gap: 0;
  padding-right: 8px;
  border-right: 1px solid #1a4a58;
  min-width: 0;
  flex-shrink: 0;
}
.brand-logo {
  font-family: 'Bungee Shade', 'Impact', 'Arial Black', sans-serif;
  font-size: 24px;
  line-height: 1;
  background: linear-gradient(180deg, #ffffff 0%, #27ffd8 22%, #00c0a8 50%, #27ffd8 75%, #e0ffff 100%);
  -webkit-background-clip: text;
  -webkit-text-fill-color: transparent;
  background-clip: text;
  filter: drop-shadow(0 0 12px rgba(39,255,216,0.55));
  letter-spacing: 2px;
}
.brand-sub {
  font-family: 'Orbitron', 'Arial Black', sans-serif;
  font-size: 8px;
  font-weight: 900;
  color: #ffd700;
  letter-spacing: 2px;
  text-transform: uppercase;
  text-shadow: 0 0 10px rgba(255,215,0,0.7), 0 0 24px rgba(255,140,0,0.35);
  white-space: nowrap;
  line-height: 1;
}

/* ── toolbar wrap ─────────────────────────── */
.toolbar { display: flex; align-items: flex-start; flex-wrap: wrap; gap: 6px; flex: 1; }

/* ── button groups ────────────────────────── */
.btn-group {
  display: flex;
  flex-direction: column;
  gap: 2px;
  background: rgba(8,18,28,0.5);
  border: 1px solid #1a3a4a;
  border-radius: 5px;
  padding: 3px 6px 4px;
}
.btn-group-label {
  font-family: 'Orbitron', monospace;
  font-size: 7px;
  font-weight: 700;
  letter-spacing: 1.5px;
  text-transform: uppercase;
  text-align: center;
  color: #27ffd8;
  opacity: 0.65;
  margin-bottom: 0;
  line-height: 1;
}
.btn-group-inner {
  display: flex;
  gap: 3px;
  align-items: center;
  flex-wrap: wrap;
}

/* ── base button reset ────────────────────── */
button {
  border-radius: 4px;
  padding: 4px 9px;
  font-size: 10px;
  font-weight: 700;
  font-family: 'Orbitron', 'Trebuchet MS', sans-serif;
  cursor: pointer;
  text-align: center;
  white-space: nowrap;
  min-width: unset;
  letter-spacing: 0.4px;
  text-transform: uppercase;
  transition: background 0.12s, box-shadow 0.12s, filter 0.12s;
  background: rgba(8,18,28,0.95);
  border: 1px solid;
  line-height: 1;
}
button:disabled { opacity: .38; cursor: default; filter: none !important; box-shadow: none !important; }

/* ── color variants ───────────────────────── */
.btn-cyan { border-color: #00d4ff; color: #00d4ff; }
.btn-cyan:not(:disabled):hover { background: rgba(0,212,255,0.14); box-shadow: 0 0 14px rgba(0,212,255,0.55); filter: brightness(1.1); }

.btn-green { border-color: #1ce8b5; color: #1ce8b5; }
.btn-green:not(:disabled):hover { background: rgba(28,232,181,0.14); box-shadow: 0 0 14px rgba(28,232,181,0.55); filter: brightness(1.1); }

.btn-magenta { border-color: #ff3390; color: #ff6db8; }
.btn-magenta:not(:disabled):hover { background: rgba(255,51,144,0.14); box-shadow: 0 0 14px rgba(255,51,144,0.55); filter: brightness(1.1); }

.btn-orange { border-color: #ff8c00; color: #ffb340; }
.btn-orange:not(:disabled):hover { background: rgba(255,140,0,0.14); box-shadow: 0 0 14px rgba(255,140,0,0.55); filter: brightness(1.1); }

.btn-gold { border-color: #ffd700; color: #ffe44d; }
.btn-gold:not(:disabled):hover { background: rgba(255,215,0,0.14); box-shadow: 0 0 14px rgba(255,215,0,0.55); filter: brightness(1.1); }

/* ── backward compat IDs (no-op now, handled by class) ─── */
#btn-save, #btn-build, #btn-connect, #btn-flash { background: rgba(8,18,28,0.95); }

#status {
  font-size: 11px;
  font-family: 'Orbitron', monospace;
  color: var(--accent);
  min-width: 180px;
  align-self: center;
  letter-spacing: 0.5px;
}
#status.warn { color: #fbbf24; }
#status.bad { color: #f87171; }

/* ── form elements ────────────────────────── */
select, input[type=text] {
  background: var(--input);
  color: var(--text);
  border: 1px solid var(--border);
  border-radius: 6px;
  padding: 6px 8px;
  font-size: 13px;
  max-width: 100%;
}

/* ── neon green sliders ───────────────────── */
input[type=range] {
  -webkit-appearance: none;
  appearance: none;
  height: 8px;
  border-radius: 4px;
  background: linear-gradient(90deg, #0a3d1a 0%, #0f5e28 100%);
  outline: none;
  cursor: pointer;
  border: 1px solid #1a6b30;
  box-shadow: 0 0 6px rgba(57,255,20,0.15);
}
input[type=range]::-webkit-slider-thumb {
  -webkit-appearance: none;
  appearance: none;
  width: 20px;
  height: 20px;
  border-radius: 50%;
  background: radial-gradient(circle at 35% 35%, #b0ff57, #39ff14 40%, #20c20e 70%, #0f7a0f);
  border: 2px solid #39ff14;
  box-shadow: 0 0 10px rgba(57,255,20,0.7), 0 0 20px rgba(57,255,20,0.3);
  cursor: pointer;
}
input[type=range]::-moz-range-thumb {
  width: 20px;
  height: 20px;
  border-radius: 50%;
  background: radial-gradient(circle at 35% 35%, #b0ff57, #39ff14 40%, #20c20e 70%, #0f7a0f);
  border: 2px solid #39ff14;
  box-shadow: 0 0 10px rgba(57,255,20,0.7), 0 0 20px rgba(57,255,20,0.3);
  cursor: pointer;
}
input[type=range]::-moz-range-track {
  height: 8px;
  border-radius: 4px;
  background: linear-gradient(90deg, #0a3d1a 0%, #0f5e28 100%);
  border: 1px solid #1a6b30;
}
input[type=range]:hover::-webkit-slider-thumb {
  box-shadow: 0 0 14px rgba(57,255,20,0.9), 0 0 28px rgba(57,255,20,0.5);
  background: radial-gradient(circle at 35% 35%, #d4ff80, #57ff2a 40%, #39ff14 70%, #1ce80f);
}
input[type=range]:active::-webkit-slider-thumb {
  transform: scale(1.15);
}
.slider-val {
  min-width: 50px;
  font-size: 12px;
  color: #39ff14;
  text-shadow: 0 0 6px rgba(57,255,20,0.5);
  font-family: 'Orbitron', Consolas, monospace;
  text-align: right;
}

.panel {
  margin: 12px 14px 0;
  background: var(--card);
  border: 1px solid var(--border);
  border-radius: 10px;
}
.tabs {
  margin: 10px 14px 0;
  display: flex;
  gap: 8px;
}
.tab-btn {
  min-width: 170px;
  border: 1px solid var(--border);
  background: linear-gradient(180deg, #0f1f2a, #091019);
  color: #bcf7ff;
  border-radius: 10px 10px 0 0;
}
.tab-btn.active {
  background: linear-gradient(180deg, #1a7f92, #0e2b36);
  color: #d5ffff;
  box-shadow: 0 0 18px rgba(39, 255, 216, 0.2);
}
.main-tab { display: none; }
.main-tab.active { display: block; }
.panel-head {
  padding: 10px 12px;
  border-bottom: 1px solid var(--border);
  color: var(--dim);
  font-size: 12px;
}

summary {
  list-style: none;
  display: flex;
  gap: 8px;
  align-items: center;
  cursor: pointer;
  padding: 10px 12px;
  border-bottom: 1px solid var(--border);
  font-weight: 700;
}
summary::before {
  content: ">";
  display: inline-block;
  width: 10px;
  color: var(--dim);
}
details[open] > summary::before {
  content: "v";
}
small.file {
  color: var(--dim);
  font-weight: 500;
}

table { width: 100%; border-collapse: collapse; table-layout: fixed; }
tr:nth-child(even) { background: rgba(255, 255, 255, 0.02); }
td { padding: 7px 10px; vertical-align: middle; }
td.name {
  width: 250px;
  min-width: 180px;
  font-family: Consolas, "Courier New", monospace;
  color: #93c5fd;
  font-size: 12px;
  word-break: break-word;
  overflow-wrap: break-word;
}
td:nth-child(2) {
  width: 340px;
  min-width: 200px;
  max-width: 420px;
  overflow: hidden;
}
td:nth-child(2) select,
td:nth-child(2) input[type=text] {
  width: 100%;
  max-width: 100%;
  box-sizing: border-box;
}
td.hint {
  min-width: 160px;
  color: var(--dim);
  font-size: 11px;
  word-break: break-word;
}

.sw {
  position: relative;
  display: inline-block;
  width: 42px;
  height: 22px;
}
.sw input { opacity: 0; width: 0; height: 0; }
.sl {
  position: absolute;
  inset: 0;
  background: #4b5563;
  border-radius: 22px;
  transition: .2s;
}
.sl::before {
  content: "";
  position: absolute;
  width: 16px;
  height: 16px;
  left: 3px;
  bottom: 3px;
  background: #fff;
  border-radius: 50%;
  transition: .2s;
}
input:checked + .sl { background: #10b981; }
input:checked + .sl::before { transform: translateX(20px); }

#log {
  position: fixed;
  left: 0;
  right: 0;
  bottom: 0;
  height: 190px;
  display: none;
  flex-direction: column;
  border-top: 2px solid var(--border);
  background: #0a0f1c;
}
#log.open { display: flex; }
#log-head {
  padding: 6px 10px;
  border-bottom: 1px solid var(--border);
  color: #86efac;
  font-size: 12px;
  display: flex;
  justify-content: space-between;
}
#log-body {
  flex: 1;
  overflow-y: auto;
  padding: 10px;
  white-space: pre-wrap;
  font: 12px Consolas, "Courier New", monospace;
  color: #bbf7d0;
}
.converter-panel {
  margin: 12px 14px 0;
  background: radial-gradient(1200px 480px at 15% -15%, #0d2a31, #08121b 45%, #05090e 100%);
  border: 1px solid var(--border);
  border-radius: 10px;
  display: flex;
  flex-direction: column;
}
.converter-head {
  display: flex;
  align-items: center;
  justify-content: space-between;
  gap: 10px;
  padding: 10px 12px;
  border-bottom: 1px solid var(--border);
}
.converter-grid {
  display: grid;
  grid-template-columns: 1fr 1fr;
  gap: 10px;
  padding: 10px;
}
.converter-actions {
  display: flex;
  gap: 8px;
  align-items: center;
}
.converter-actions button {
  min-width: auto;
  padding: 5px 10px;
  text-align: center;
  border: 1px solid var(--border);
  color: #27ffd8;
  background: rgba(8,18,28,0.95);
}
.converter-actions button:hover {
  border-color: #27ffd8;
  box-shadow: 0 0 10px rgba(39,255,216,0.4);
}
.converter-note {
  color: var(--dim);
  font-size: 12px;
  padding: 8px 12px;
}
.converter-pane {
  border: 1px solid var(--border);
  border-radius: 8px;
  background: #061018;
  padding: 10px;
}
.converter-pane.hidden { display: none; }
.converter-pane h3 {
  font-size: 14px;
  color: #7cf3ff;
  margin-bottom: 8px;
}
.converter-pane textarea {
  width: 100%;
  min-height: 240px;
  resize: vertical;
  background: #04080d;
  color: #c8f8ff;
  border: 1px solid var(--border);
  border-radius: 6px;
  padding: 8px;
  font: 12px Consolas, "Courier New", monospace;
}
.converter-row {
  display: flex;
  gap: 8px;
  margin-bottom: 8px;
  align-items: center;
  flex-wrap: wrap;
}
.converter-actions button.active-mode {
  outline: 2px solid var(--accent);
  border-color: var(--accent);
  box-shadow: 0 0 14px rgba(39,255,216,0.6);
}
.chrome-chip {
  display: inline-block;
  padding: 2px 8px;
  border: 1px solid #1f7282;
  border-radius: 999px;
  color: #98f8ff;
  background: rgba(15, 70, 83, 0.25);
  font-size: 11px;
}
@media (max-width: 980px) {
  td.name { width: 230px; }
  .converter-grid { grid-template-columns: 1fr; }
}

/* Build-failure modal */
#buildFailModal {
  display: none; position: fixed; inset: 0; z-index: 9999;
  background: rgba(0,0,0,0.7); align-items: center; justify-content: center;
}
#buildFailModal.visible { display: flex; }
#buildFailModal .modal-box {
  background: #1a1a2e; border: 2px solid #f87171; border-radius: 12px;
  padding: 24px 28px; max-width: 520px; width: 90%; color: #e2e8f0;
  box-shadow: 0 0 40px rgba(248,113,113,0.3);
}
#buildFailModal h2 { color: #f87171; margin: 0 0 12px; font-size: 18px; }
#buildFailModal ul { padding-left: 18px; margin: 10px 0 18px; line-height: 1.7; font-size: 13px; color: #cbd5e1; }
#buildFailModal .modal-actions { display: flex; gap: 10px; flex-wrap: wrap; margin-top: 14px; }
#buildFailModal .modal-actions button { flex: 1; min-width: 100px; padding: 8px 12px; border-radius: 6px; font-size: 13px; cursor: pointer; border: 1px solid; background: transparent; }
#buildFailModal .mbtn-reset { border-color: #f87171; color: #f87171; }
#buildFailModal .mbtn-reset:hover { background: rgba(248,113,113,0.15); }
#buildFailModal .mbtn-log { border-color: #fbbf24; color: #fbbf24; }
#buildFailModal .mbtn-log:hover { background: rgba(251,191,36,0.15); }
#buildFailModal .mbtn-close { border-color: #64748b; color: #94a3b8; }
#buildFailModal .mbtn-close:hover { background: rgba(100,116,139,0.2); }
</style>
</head>
<body>

<!-- Build-failure troubleshooting modal -->
<div id="buildFailModal">
  <div class="modal-box">
    <h2>&#9888; Build Failed</h2>
    <p style="font-size:13px;color:#94a3b8;margin:0 0 8px">Don't panic! Here are some things to try:</p>
    <ul>
      <li><strong>Reset the vehicle</strong> &mdash; restores all settings to their original defaults</li>
      <li><strong>Check the build log</strong> &mdash; scroll to the bottom to see the actual error</li>
      <li>If you just installed a custom sound, it may be <strong>too large</strong> &mdash; try a shorter loop or lower sample rate</li>
      <li>Make sure no slider values are blank or set to something weird</li>
      <li>Try switching to a different vehicle and building that &mdash; if it works, the problem is in your vehicle settings</li>
    </ul>
    <div class="modal-actions">
      <button class="mbtn-reset" onclick="closeBuildFailModal(); resetVehicleNow()">&#8634; Reset Vehicle</button>
      <button class="mbtn-log" onclick="closeBuildFailModal(); document.getElementById('log').className='open'">&#128220; View Build Log</button>
      <button class="mbtn-close" onclick="closeBuildFailModal()">Close</button>
    </div>
  </div>
</div>

<header>
  <!-- ══ Brand ══════════════════════════════════════════════════════ -->
  <div class="brand">
    <div class="brand-logo">DIYGuy999</div>
    <div class="brand-sub">&#9670; Light &middot; Sound &middot; Speed Controller &#9670;</div>
    <div id="currentVehicleName" style="font-family:'Orbitron',monospace;font-size:12px;font-weight:700;color:#ffd700;letter-spacing:1px;margin-top:3px;white-space:nowrap;overflow:hidden;text-overflow:ellipsis;max-width:320px;text-shadow:0 0 8px rgba(255,215,0,0.4)" title="Currently active vehicle"></div>
  </div>

  <!-- ══ Toolbar ════════════════════════════════════════════════════ -->
  <div class="toolbar">

    <!-- FLASH & BUILD group -->
    <div class="btn-group">
      <div class="btn-group-label">&#128640; Flash &amp; Build</div>
      <div class="btn-group-inner">
        <select id="portSelect" style="max-width:120px" title="USB/COM port for your ESP32 board"></select>
        <button id="btn-refresh-ports" class="btn-cyan" onclick="loadPorts()" title="Rescan USB ports">&#8635;</button>
        <button id="btn-connect" class="btn-cyan" onclick="connectBoard()" title="Connect to selected port">&#9889; Connect</button>
        <button id="btn-build" class="btn-orange" onclick="runCmd('build')" title="Compile firmware (no upload)">&#9881; Build</button>
        <button id="btn-flash" class="btn-orange" onclick="runCmd('flash')" title="Build and upload to board">&#128640; Flash</button>
        <span id="buildLight" title="Build status" style="display:inline-block;width:12px;height:12px;border-radius:50%;background:#333;border:1px solid #555;vertical-align:middle;margin-left:2px"></span>
      </div>
    </div>

    <!-- VEHICLE ACTIONS group -->
    <div class="btn-group">
      <div class="btn-group-label">&#128203; Vehicle</div>
      <div class="btn-group-inner">
        <button class="btn-cyan" onclick="saveAll()" title="Save all changes now">&#128190; Save</button>
        <button class="btn-magenta" onclick="resetVehicleNow()" title="Restore vehicle to original defaults">&#8634; Reset</button>
        <button class="btn-green" onclick="createVehicleFromCurrent()" title="Create a new vehicle from the current one">&#10010; Clone</button>
        <button class="btn-gold" onclick="downloadVehicleFile()" title="Download the active vehicle .h file">&#128229; Export</button>
      </div>
    </div>

    <!-- TOOLS group -->
    <div class="btn-group">
      <div class="btn-group-label">&#127925; Tools</div>
      <div class="btn-group-inner">
        <button id="btn-converter" class="btn-gold" onclick="openConverter()" title="Open the Sound Forge editor">&#127925; Sound Forge</button>
        <button id="btn-open-ide" class="btn-cyan" onclick="openArduinoIDE()" title="Open source code in Arduino IDE">&#128187; IDE</button>
      </div>
    </div>

    <!-- OPTIONS group -->
    <div class="btn-group">
      <div class="btn-group-label">&#9881; Options</div>
      <div class="btn-group-inner">
        <label style="display:inline-flex;align-items:center;gap:5px;font-size:11px;color:#d4f7ff;white-space:nowrap" title="Show or hide advanced sound tuning settings">
          <input id="advSoundToggle" type="checkbox" onchange="setAdvancedSoundVisibility(this.checked)">
          <span>Show Advanced Sound Tuning</span>
        </label>
      </div>
    </div>

    <!-- VOLUME group -->
    <div class="btn-group">
      <div class="btn-group-label">&#128266; Volume</div>
      <div class="btn-group-inner" style="min-width:160px">
        <input id="masterVolSlider" type="range" min="0" max="250" value="200" step="5"
          style="width:110px"
          oninput="onVolumeSliderChange(this.value)"
          onchange="saveVolumeSlider(this.value)"
          title="Master volume (0-250%)">
        <span id="masterVolLabel" style="font-size:11px;color:#20C20E;min-width:32px">200%</span>
        <label style="font-size:10px;color:#fbbf24;cursor:pointer;white-space:nowrap;margin-left:6px" title="Bypass RC volume pot — use only this slider">
          <input id="volPotOverride" type="checkbox" onchange="saveVolPotOverride(this.checked)"> Pot Override
        </label>
      </div>
    </div>

    <span id="status">All settings saved</span>
  </div>
</header>

<div class="tabs">
  <button id="tabBtnConfig" class="tab-btn active" onclick="switchMainTab('config')">Truck Setup</button>
  <button id="tabBtnConverter" class="tab-btn" onclick="switchMainTab('converter')">Live Sound Builder</button>
</div>

<div id="mainTabConfig" class="main-tab active">
  <div class="panel">
    <div class="panel-head" data-i18n="panelHint">Sections start closed. Open only the section you need.</div>
    __SECTIONS__
  </div>
</div>

<div id="mainTabConverter" class="main-tab">
  <div id="converterPanel" class="converter-panel" role="region" aria-label="Live Sound Builder">
    <div class="converter-head">
      <strong>Live Sound Builder</strong>
      <span class="chrome-chip">Sound Forge</span>
    </div>
    <div class="converter-note">Browse, preview &amp; tune all 500+ sounds live, or convert your own WAV files.</div>
    <div class="converter-grid">
      <div>
        <div class="converter-pane" style="margin-bottom:10px">
          <h3>&#127925; Live Sound Editor</h3>
          <div style="padding:6px;background:#0a0a1a;border-radius:8px;border:1px solid #333">
            <div style="display:flex;align-items:center;gap:12px;flex-wrap:wrap">
              <span style="color:#fbbf24;font-size:13px;font-weight:bold" id="sbNowPlaying">No sound loaded</span>
              <button type="button" class="btn-cyan" onclick="sbPlayStop()" id="sbPlayBtn" style="min-width:60px">&#9654; Play</button>
              <label style="font-size:12px;color:#ccc"><input id="sbLoop" type="checkbox" checked> Loop</label>
            </div>
            <div style="display:flex;align-items:center;gap:10px;margin-top:8px;flex-wrap:wrap">
              <label style="color:#93c5fd;font-size:12px;white-space:nowrap">RPM:</label>
              <input id="sbRpmSlider" type="range" min="0.3" max="3.0" step="0.05" value="1.0"
                style="flex:1;min-width:150px"
                oninput="sbUpdateRpm(this.value)">
              <span id="sbRpmLabel" style="color:#fbbf24;font-size:13px;min-width:70px">1.00x (idle)</span>
            </div>
            <div style="display:flex;align-items:center;gap:10px;margin-top:6px;flex-wrap:wrap">
              <label style="color:#93c5fd;font-size:12px;white-space:nowrap">Volume:</label>
              <input id="sbVolSlider" type="range" min="0" max="200" step="5" value="100"
                style="width:100px"
                oninput="sbUpdateVol(this.value)">
              <span id="sbVolLabel" style="color:#20C20E;font-size:12px">100%</span>
              <span style="color:#666;font-size:11px;margin-left:auto" id="sbSoundInfo"></span>
            </div>
            <div style="display:flex;align-items:center;gap:10px;margin-top:8px;flex-wrap:wrap">
              <label style="color:#4ade80;font-size:12px;white-space:nowrap">Loop Start:</label>
              <input id="sbLoopStart" type="range" min="0" max="1" step="0.001" value="0"
                style="flex:1;min-width:120px"
                oninput="sbUpdateLoopPoints()">
              <span id="sbLoopStartLabel" style="color:#4ade80;font-size:11px;min-width:50px">0.000s</span>
            </div>
            <div style="display:flex;align-items:center;gap:10px;margin-top:4px;flex-wrap:wrap">
              <label style="color:#f87171;font-size:12px;white-space:nowrap">Loop End:</label>
              <input id="sbLoopEnd" type="range" min="0" max="1" step="0.001" value="1"
                style="flex:1;min-width:120px"
                oninput="sbUpdateLoopPoints()">
              <span id="sbLoopEndLabel" style="color:#f87171;font-size:11px;min-width:50px">1.000s</span>
            </div>
            <div style="display:flex;align-items:center;gap:10px;margin-top:6px;flex-wrap:wrap">
              <label style="color:#fbbf24;font-size:12px;white-space:nowrap">Smooth:</label>
              <input id="sbSmooth" type="range" min="0" max="100" step="5" value="0"
                style="flex:1;min-width:100px"
                oninput="document.getElementById('sbSmoothLabel').textContent=this.value+'%'; if(_sbPlaying){if(_sbSwapTimer)clearTimeout(_sbSwapTimer);_sbSwapTimer=setTimeout(_sbHotSwap,150);}">
              <span id="sbSmoothLabel" style="color:#fbbf24;font-size:11px;min-width:30px">0%</span>
              <span style="color:#666;font-size:10px">(evens out loud &amp; quiet spots)</span>
            </div>
            <div style="display:flex;align-items:center;gap:10px;margin-top:6px;flex-wrap:wrap">
              <label style="color:#f472b6;font-size:12px;white-space:nowrap">Crossfade:</label>
              <input id="sbCrossfade" type="range" min="0" max="100" step="1" value="10"
                style="flex:1;min-width:100px"
                oninput="document.getElementById('sbCrossfadeLabel').textContent=this.value+'%'; if(_sbPlaying){if(_sbSwapTimer)clearTimeout(_sbSwapTimer);_sbSwapTimer=setTimeout(_sbHotSwap,150);}">
              <span id="sbCrossfadeLabel" style="color:#f472b6;font-size:11px;min-width:30px">10%</span>
              <span style="color:#666;font-size:10px">(0=off, blends end&rarr;start for seamless loop)</span>
            </div>
            <div style="display:flex;align-items:center;gap:10px;margin-top:6px;flex-wrap:wrap">
              <label style="color:#60a5fa;font-size:12px;white-space:nowrap">Low Cut:</label>
              <input id="sbHighPass" type="range" min="0" max="2000" step="10" value="0"
                style="flex:1;min-width:100px"
                oninput="document.getElementById('sbHighPassLabel').textContent=this.value+'Hz'; sbUpdateFilters();">
              <span id="sbHighPassLabel" style="color:#60a5fa;font-size:11px;min-width:40px">0Hz</span>
              <span style="color:#666;font-size:10px">(removes rumble)</span>
            </div>
            <div style="display:flex;align-items:center;gap:10px;margin-top:6px;flex-wrap:wrap">
              <label style="color:#a78bfa;font-size:12px;white-space:nowrap">High Cut:</label>
              <input id="sbLowPass" type="range" min="500" max="11025" step="25" value="11025"
                style="flex:1;min-width:100px"
                oninput="document.getElementById('sbLowPassLabel').textContent=this.value+'Hz'; sbUpdateFilters();">
              <span id="sbLowPassLabel" style="color:#a78bfa;font-size:11px;min-width:50px">11025Hz</span>
              <span style="color:#666;font-size:10px">(removes hiss)</span>
            </div>
            <div style="display:flex;align-items:center;gap:10px;margin-top:6px;flex-wrap:wrap">
              <label style="color:#e879f9;font-size:12px;white-space:nowrap">Pitch:</label>
              <input id="sbPitch" type="range" min="-12" max="12" step="0.5" value="0"
                style="flex:1;min-width:100px"
                oninput="sbUpdatePitch()">
              <span id="sbPitchLabel" style="color:#e879f9;font-size:11px;min-width:50px">0 st</span>
              <label style="font-size:11px;color:#ccc;white-space:nowrap" title="When locked, pitch stays constant regardless of RPM speed"><input id="sbPitchLock" type="checkbox" onchange="sbUpdatePitch()"> Lock</label>
              <span style="color:#666;font-size:10px">(semitones)</span>
            </div>
            <p style="color:#888;font-size:11px;margin:8px 0 0;padding-top:8px;border-top:1px solid #333">Export bakes in your current RPM, loop points, and pitch settings.</p>
            <div style="display:flex;align-items:center;gap:8px;margin-top:6px;flex-wrap:wrap">
              <label style="color:#22d3ee;font-size:12px;white-space:nowrap">Rate:</label>
              <select id="sbExportRate" style="width:80px" title="Output sample rate">
                <option value="8000">8 kHz</option>
                <option value="11025">11 kHz</option>
                <option value="16000">16 kHz</option>
                <option value="22050" selected>22 kHz</option>
              </select>
              <label style="font-size:12px;color:#ccc"><input id="sbExportNorm" type="checkbox" checked> Normalize</label>
            </div>
            <div style="display:flex;align-items:center;gap:8px;margin-top:8px;flex-wrap:wrap">
              <label style="color:#20C20E;font-size:12px;white-space:nowrap">Install as:</label>
              <select id="sbInstallCategory" style="flex:1;min-width:200px"><option value="">-- loading categories --</option></select>
            </div>
            <div style="display:flex;align-items:center;gap:8px;margin-top:8px;flex-wrap:wrap">
              <button type="button" class="btn-gold" onclick="sbExportSelection()" title="Export the selected loop region as a .h header file">&#128229; Export .h</button>
              <button type="button" class="btn-gold" onclick="sbInstallSelection()" title="Export and install to active vehicle">&#9654; Install</button>
              <span id="sbSelectionInfo" style="color:#93c5fd;font-size:11px"></span>
            </div>
          </div>
          <div style="margin-top:10px;border-top:1px solid #333;padding-top:8px">
            <h4 onclick="var p=document.getElementById('synthPanel');var a=this.querySelector('span');if(p.style.display==='none'){p.style.display='block';a.textContent='▾'}else{p.style.display='none';a.textContent='▸'}" style="cursor:pointer;color:#22d3ee;margin:0;font-size:13px;user-select:none">&#9881; Engine Synthesizer <span style="font-size:11px">&#9656;</span></h4>
            <div id="synthPanel" style="display:none;margin-top:8px">
              <p style="color:#93c5fd;font-size:11px;margin:0 0 8px">Generate engine sounds from scratch. Each cylinder fires individually per the real firing order.</p>
              <div style="display:flex;gap:8px;flex-wrap:wrap;align-items:center;margin-bottom:6px">
                <label style="color:#22d3ee;font-size:12px;white-space:nowrap">Engine:</label>
                <select id="synthEngine" style="min-width:160px" onchange="synthUpdateLabel()">
                  <option value="single">Single Cylinder</option>
                  <option value="twin">Parallel Twin</option>
                  <option value="i3">Inline 3</option>
                  <option value="i4">Inline 4</option>
                  <option value="boxer4">Flat 4 (Boxer/Subaru)</option>
                  <option value="i6" selected>Inline 6</option>
                  <option value="v6">V6 (60&deg;)</option>
                  <option value="v8xp">V8 Cross-plane</option>
                  <option value="v8fp">V8 Flat-plane (Ferrari)</option>
                  <option value="v10">V10</option>
                  <option value="v12">V12</option>
                </select>
                <span id="synthOrderLabel" style="color:#666;font-size:10px"></span>
              </div>
              <div style="display:flex;gap:10px;flex-wrap:wrap;align-items:center;margin-bottom:4px">
                <label style="color:#fbbf24;font-size:12px;white-space:nowrap">RPM:</label>
                <input id="synthRpm" type="range" min="300" max="8000" step="50" value="800" style="flex:1;min-width:120px" oninput="document.getElementById('synthRpmVal').textContent=this.value">
                <span id="synthRpmVal" style="color:#fbbf24;font-size:12px;min-width:40px">800</span>
              </div>
              <div style="display:flex;gap:10px;flex-wrap:wrap;align-items:center;margin-bottom:4px">
                <label style="color:#f87171;font-size:12px;white-space:nowrap">Combustion:</label>
                <input id="synthCombustion" type="range" min="10" max="100" step="5" value="75" style="flex:1;min-width:80px" oninput="document.getElementById('synthCombVal').textContent=this.value+'%'">
                <span id="synthCombVal" style="color:#f87171;font-size:11px;min-width:30px">75%</span>
                <span style="color:#666;font-size:10px">(explosion energy)</span>
              </div>
              <div style="display:flex;gap:10px;flex-wrap:wrap;align-items:center;margin-bottom:4px">
                <label style="color:#fb923c;font-size:12px;white-space:nowrap">Exhaust Hz:</label>
                <input id="synthExhaustFreq" type="range" min="60" max="500" step="10" value="180" style="flex:1;min-width:80px" oninput="document.getElementById('synthExhVal').textContent=this.value+'Hz'">
                <span id="synthExhVal" style="color:#fb923c;font-size:11px;min-width:45px">180Hz</span>
                <span style="color:#666;font-size:10px">(deep=low, harsh=high)</span>
              </div>
              <div style="display:flex;gap:10px;flex-wrap:wrap;align-items:center;margin-bottom:4px">
                <label style="color:#38bdf8;font-size:12px;white-space:nowrap">Injector:</label>
                <input id="synthInjector" type="range" min="0" max="100" step="5" value="25" style="flex:1;min-width:80px" oninput="document.getElementById('synthInjVal').textContent=this.value+'%'">
                <span id="synthInjVal" style="color:#38bdf8;font-size:11px;min-width:30px">25%</span>
                <span style="color:#666;font-size:10px">(click before fire)</span>
              </div>
              <div style="display:flex;gap:10px;flex-wrap:wrap;align-items:center;margin-bottom:4px">
                <label style="color:#a3e635;font-size:12px;white-space:nowrap">Mechanical:</label>
                <input id="synthMechanical" type="range" min="0" max="100" step="5" value="20" style="flex:1;min-width:80px" oninput="document.getElementById('synthMechVal').textContent=this.value+'%'">
                <span id="synthMechVal" style="color:#a3e635;font-size:11px;min-width:30px">20%</span>
                <span style="color:#666;font-size:10px">(valvetrain ticks)</span>
              </div>
              <div style="display:flex;gap:10px;flex-wrap:wrap;align-items:center;margin-bottom:4px">
                <label style="color:#e879f9;font-size:12px;white-space:nowrap">Roughness:</label>
                <input id="synthRoughness" type="range" min="0" max="100" step="5" value="15" style="flex:1;min-width:80px" oninput="document.getElementById('synthRoughVal').textContent=this.value+'%'">
                <span id="synthRoughVal" style="color:#e879f9;font-size:11px;min-width:30px">15%</span>
                <span style="color:#666;font-size:10px">(cylinder variation)</span>
              </div>
              <div style="display:flex;gap:10px;flex-wrap:wrap;align-items:center;margin-bottom:4px">
                <label style="color:#c084fc;font-size:12px;white-space:nowrap">Resonance:</label>
                <input id="synthResonance" type="range" min="1" max="20" step="0.5" value="5" style="flex:1;min-width:80px">
                <span id="synthResVal" style="color:#c084fc;font-size:11px;min-width:25px">5</span>
                <span style="color:#666;font-size:10px">(exhaust pipe Q)</span>
              </div>
              <div style="display:flex;gap:10px;flex-wrap:wrap;align-items:center;margin-bottom:4px">
                <label style="color:#f472b6;font-size:12px;white-space:nowrap">Drive:</label>
                <input id="synthDrive" type="range" min="0" max="100" step="5" value="40" style="flex:1;min-width:80px">
                <span id="synthDriveVal" style="color:#f472b6;font-size:11px;min-width:30px">40%</span>
                <span style="color:#666;font-size:10px">(warm saturation)</span>
              </div>
              <div style="display:flex;gap:10px;flex-wrap:wrap;align-items:center;margin-bottom:4px">
                <label style="color:#94a3b8;font-size:12px;white-space:nowrap">Stroke:</label>
                <select id="synthStroke" style="width:90px">
                  <option value="4" selected>4-stroke</option>
                  <option value="2">2-stroke</option>
                </select>
                <label style="color:#94a3b8;font-size:12px;white-space:nowrap">Cycles:</label>
                <select id="synthCycles" style="width:50px">
                  <option value="1">1</option>
                  <option value="2" selected>2</option>
                  <option value="4">4</option>
                </select>
                <span style="color:#666;font-size:10px">(more = longer loop)</span>
              </div>
              <div style="display:flex;gap:8px;flex-wrap:wrap;align-items:center;margin-top:8px">
                <button type="button" class="btn-cyan" onclick="synthGenerateEngine()" style="font-weight:bold">&#9881; Generate Engine</button>
                <span id="synthStatus" style="color:#4ade80;font-size:11px"></span>
              </div>
            </div>
          </div>
        </div>
        <div id="paneAudio" class="converter-pane">
          <h3>&#128196; WAV to Header Converter</h3>
          <div class="converter-row">
            <input id="convWavFile" type="file" accept=".wav,audio/wav">
            <input id="convVarName" type="text" value="" placeholder="pick a category first" readonly style="background:#1a1a2e;color:#888;cursor:not-allowed">
            <select id="convOutRate">
              <option value="8000">8000 Hz</option>
              <option value="11025">11025 Hz</option>
              <option value="16000">16000 Hz</option>
              <option value="22050" selected>22050 Hz</option>
              <option value="32000">32000 Hz</option>
            </select>
            <select id="convSpeed" title="Playback speed: slower = deeper/longer, faster = higher/shorter">
              <option value="0.25">0.25x (very slow)</option>
              <option value="0.5">0.5x (half speed)</option>
              <option value="0.75">0.75x (slower)</option>
              <option value="1" selected>1x (normal)</option>
              <option value="1.25">1.25x (faster)</option>
              <option value="1.5">1.5x</option>
              <option value="2">2x (double speed)</option>
            </select>
          </div>
          <div class="converter-row">
            <label style="color:#fbbf24;font-size:12px" title="Automatically trim audio to fit in flash memory"><input id="convAutoTrim" type="checkbox" checked> Auto trim to</label>
            <input id="convAutoTrimMax" type="range" min="0.5" max="10" step="0.5" value="2" style="width:100px" title="Maximum duration in seconds" oninput="this.nextElementSibling.textContent=this.value+'s'">
            <span class="slider-val">2s</span>
            <span id="convDuration" style="color:#20C20E;font-size:11px;margin-left:8px"></span>
          </div>
          <div class="converter-row">
            <label><input id="convNormalize" type="checkbox" checked> Normalize</label>
            <label style="margin-left:12px"><input id="convLoopFade" type="checkbox" checked> Loop crossfade</label>
            <button type="button" class="btn-cyan" onclick="convertWavToHeader()" data-i18n="convertNow">Convert Now</button>
            <button type="button" class="btn-gold" onclick="downloadHeaderText()" data-i18n="downloadHeader">Download .h</button>
            <button type="button" id="btn-install-header" class="btn-gold" onclick="installHeaderToSrc()" data-i18n-title="tooltipInstallHeader" title="Save converted .h into sounds/ and add it to the active vehicle's sound dropdown" data-i18n="installHeaderTitle">&#9654; Install Sound</button>
          </div>
          <div class="converter-row">
            <label style="color:#20C20E">Install as:</label>
            <select id="convSoundCategory" style="flex:1;min-width:200px" onchange="onCategoryChanged(this)"><option value="">-- loading categories --</option></select>
          </div>
          <textarea id="convHeaderOut" placeholder="Converted header will appear here..."></textarea>
          <div id="customSoundsPanel" style="margin-top:12px;border-top:1px solid #333;padding-top:10px">
            <h4 style="color:#20C20E;margin:0 0 8px 0;font-size:13px">&#128465; Installed Custom Sounds</h4>
            <div id="customSoundsList" style="font-size:12px;color:#ccc">Loading...</div>
          </div>
        </div>
      </div>
      <div>
        <div id="paneBrowser" class="converter-pane">
          <h3>&#127911; Sound Browser</h3>
          <p style="color:#93c5fd;font-size:12px;margin:4px 0 10px">Browse all 500+ sounds. Click a sound to load it into the editor.</p>
          <div class="converter-row" style="flex-wrap:wrap;gap:8px">
            <select id="sbCatFilter" onchange="filterSoundBrowser()" style="min-width:150px">
              <option value="">All categories</option>
              <option value="idle">Idle</option>
              <option value="rev">Rev / Throttle</option>
              <option value="start">Start</option>
              <option value="knock">Knock / Ignition</option>
              <option value="jakebrake">Jake Brake</option>
              <option value="horn">Horn</option>
              <option value="siren">Siren / Music</option>
              <option value="airbrake">Air Brake</option>
              <option value="parking">Parking Brake</option>
              <option value="shifting">Gear Shifting</option>
              <option value="turbo">Turbo</option>
              <option value="wastegate">Wastegate / Blowoff</option>
              <option value="fan">Fan</option>
              <option value="indicator">Indicator</option>
              <option value="reversing">Reversing</option>
              <option value="coupling">Coupling</option>
              <option value="hydraulicpump">Hydraulic Pump</option>
              <option value="hydraulicflow">Hydraulic Flow</option>
              <option value="trackrattle">Track Rattle</option>
              <option value="bucketrattle">Bucket Rattle</option>
              <option value="supercharger">Supercharger</option>
              <option value="other">Other / Misc</option>
            </select>
            <input id="sbSearch" type="text" placeholder="Search sounds..." oninput="filterSoundBrowser()" style="flex:1;min-width:150px">
            <span id="sbCount" style="color:#20C20E;font-size:12px"></span>
          </div>
          <div id="sbList" style="max-height:600px;overflow-y:auto;border:1px solid #333;border-radius:6px;background:#0d0d1a;margin-top:8px"></div>
        </div>
      </div>
    </div>
  </div>
</div>

<div id="log">
  <div id="log-head">
    <span id="log-title" data-i18n="logTitle">Build Output</span>
    <button onclick="closeLog()" style="background:none;color:#9ca3af;padding:0 4px;min-width:20px;text-align:center">X</button>
  </div>
  <div id="log-body"></div>
</div>

<script>
let dirty = false;
let connectedPort = null;
let showAdvancedSound = false;
const vehicleDrafts = {};
let convertedWavBlob = null;


function status(text, cls) {
  const s = document.getElementById('status');
  s.textContent = text;
  s.className = cls || '';
}

function refreshStatusLabel() {
  if (dirty) {
    status('Unsaved changes', 'warn');
    return;
  }
  if (connectedPort) {
    status('Connected to ' + connectedPort, '');
    return;
  }
  status('All settings saved', '');
}

function getSelectedVehicleFile() {
  const sel = document.querySelector('form[data-file="1_Vehicle.h"] select[name="__vehicle__"]');
  return (sel && sel.value) ? sel.value : null;
}

function updateVehicleNameDisplay() {
  const el = document.getElementById('currentVehicleName');
  if (!el) return;
  const veh = getSelectedVehicleFile();
  if (veh) {
    const nice = veh.replace(/\.h$/i, '').replace(/([a-z])([A-Z])/g, '$1 $2').replace(/_/g, ' ');
    el.textContent = '\u25B6 ' + nice;
    el.title = 'Active vehicle: ' + veh;
  } else {
    el.textContent = '';
  }
}

async function resetVehicleNow() {
  const vehicle = getSelectedVehicleFile();
  if (!vehicle) { status('No vehicle selected', 'bad'); return; }
  if (!confirm('Reset ' + vehicle + ' to its original defaults? This cannot be undone.')) return;
  try {
    const r = await fetch('/reset_vehicle', {
      method: 'POST',
      headers: {'Content-Type': 'application/json'},
      body: JSON.stringify({vehicle: vehicle})
    });
    const j = await r.json();
    if (!j.ok) throw new Error(j.error || 'reset failed');
    // Reload the vehicle section to reflect the restored file
    const vehicleSel = document.querySelector('form[data-file="1_Vehicle.h"] select[name="__vehicle__"]');
    if (vehicleSel) await handleVehicleSelection(vehicleSel);
    status('Vehicle reset to defaults: ' + vehicle + ' \\u2714', '');
  } catch(e) { status('Reset failed: ' + e, 'bad'); }
}

async function createVehicleFromCurrent() {
  const vehicle = getSelectedVehicleFile();
  if (!vehicle) { status('No vehicle selected', 'bad'); return; }
  const base = vehicle.replace(/\\.h$/i, '');
  const newName = prompt('Name for new vehicle (letters, numbers, _ only):', base + '_copy');
  if (!newName) return;
  try {
    const r = await fetch('/export_vehicle', {
      method: 'POST',
      headers: {'Content-Type': 'application/json'},
      body: JSON.stringify({vehicle: vehicle, newName: newName})
    });
    const j = await r.json();
    if (!j.ok) throw new Error(j.error || 'create failed');
    // Add to dropdown and switch instantly
    const sel = document.querySelector('form[data-file="1_Vehicle.h"] select[name="__vehicle__"]');
    if (sel) {
      const opt = document.createElement('option');
      opt.value = j.newFile;
      opt.textContent = j.newFile.replace(/\\.h$/i, '');
      sel.appendChild(opt);
      sel.value = j.newFile;
      await handleVehicleSelection(sel);
    }
    localStorage.setItem('lastVehicle', j.newFile);
    status('Created ' + j.newFile + ' \\u2714', '');
  } catch(e) { status('Create failed: ' + e, 'bad'); }
}

async function downloadVehicleFile() {
  const vehicle = getSelectedVehicleFile();
  if (!vehicle) { status('No vehicle selected', 'bad'); return; }
  try {
    const r = await fetch('/download_vehicle?vehicle=' + encodeURIComponent(vehicle));
    if (!r.ok) throw new Error('Download failed');
    const blob = await r.blob();
    const a = document.createElement('a');
    a.href = URL.createObjectURL(blob);
    a.download = vehicle;
    a.click();
    URL.revokeObjectURL(a.href);
    status('Downloaded ' + vehicle, '');
  } catch(e) { status('Export failed: ' + e, 'bad'); }
}

function applyFormDataMapToDom(dataMap) {
  Object.keys(dataMap || {}).forEach(file => {
    const form = document.querySelector('form[data-file="' + file + '"]');
    if (!form) return;
    const fields = dataMap[file] || {};

    Object.keys(fields).forEach(name => {
      const info = fields[name] || {};
      const kind = info.kind || '';

      if (kind === 'flag') {
        const cb = form.querySelector('input[name="' + name + '"]');
        if (cb) cb.checked = !!info.enabled;
        return;
      }

      if (kind === 'bool_var') {
        const cb = form.querySelector('input[name="' + name + '"]');
        if (cb) cb.checked = String(info.value).toLowerCase() === 'true';
        return;
      }

      if (kind === 'define_val') {
        const en = form.querySelector('input[name="' + name + '__enabled"]');
        const tx = form.querySelector('[name="' + name + '"]');
        if (en) en.checked = !!info.enabled;
        if (tx) tx.value = info.value || '';
        return;
      }

      if (kind === 'text_var') {
        const tx = form.querySelector('[name="' + name + '"]');
        if (tx) {
          tx.value = info.value || '';
          // Update range slider label if present
          if (tx.type === 'range' && tx.nextElementSibling) {
            const suffix = tx.nextElementSibling.textContent.replace(/[0-9-]/g, '');
            tx.nextElementSibling.textContent = tx.value + suffix;
          }
        }
        return;
      }

      if (kind === 'sound_choice') {
        const sx = form.querySelector('select[name="' + name + '"]');
        if (sx) sx.value = info.value || '';
      }
    });
  });
}

let _autoSaveTimer = null;
function markDirty() {
  dirty = true;
  refreshStatusLabel();
  // Auto-save after 1.5 seconds of no changes
  if (_autoSaveTimer) clearTimeout(_autoSaveTimer);
  _autoSaveTimer = setTimeout(() => { _autoSaveTimer = null; saveAll(); }, 1500);
}

function setAdvancedSoundVisibility(show) {
  showAdvancedSound = !!show;
  document.querySelectorAll('tr.adv-sound').forEach(row => {
    row.style.display = showAdvancedSound ? '' : 'none';
  });
}

function openConverter() {
  switchMainTab('converter');
  loadSoundCategories();
  loadCustomSounds();
}

function switchMainTab(tab) {
  const isConverter = tab === 'converter';
  const cfg = document.getElementById('mainTabConfig');
  const conv = document.getElementById('mainTabConverter');
  const bCfg = document.getElementById('tabBtnConfig');
  const bConv = document.getElementById('tabBtnConverter');
  if (!cfg || !conv || !bCfg || !bConv) return;
  cfg.className = isConverter ? 'main-tab' : 'main-tab active';
  conv.className = isConverter ? 'main-tab active' : 'main-tab';
  bCfg.className = isConverter ? 'tab-btn' : 'tab-btn active';
  bConv.className = isConverter ? 'tab-btn active' : 'tab-btn';
  if (isConverter) loadSoundBrowser();
}

function openConverterMode(mode) {
  // Legacy compatibility — no-op, both panes are always visible now
}

function previewHeaderToWavBlob(text) {
  const mRate = text.match(/sample\s*rate\s*=\s*(\d+)/i) || text.match(/samplerate\s*=\s*(\d+)/i);
  const rate = mRate ? parseInt(mRate[1], 10) : 22050;
  const bodyMatch = text.match(/\{([\s\S]*?)\}/m);
  if (!bodyMatch) throw new Error('No sample array found in header file');
  const values = (bodyMatch[1].match(/-?\d+/g) || []).map(v => Math.max(-128, Math.min(127, parseInt(v, 10))));
  const dataLen = values.length;
  const wav = new Uint8Array(44 + dataLen);
  const writeAscii = (off, s) => { for (let i = 0; i < s.length; i++) wav[off + i] = s.charCodeAt(i); };
  const write16 = (off, v) => { wav[off] = v & 255; wav[off+1] = (v >> 8) & 255; };
  const write32 = (off, v) => { wav[off] = v & 255; wav[off+1] = (v>>8)&255; wav[off+2]=(v>>16)&255; wav[off+3]=(v>>24)&255; };
  writeAscii(0, 'RIFF');
  write32(4, 36 + dataLen);
  writeAscii(8, 'WAVE');
  writeAscii(12, 'fmt ');
  write32(16, 16);
  write16(20, 1);
  write16(22, 1);
  write32(24, rate);
  write32(28, rate);
  write16(32, 1);
  write16(34, 8);
  writeAscii(36, 'data');
  write32(40, dataLen);
  for (let i = 0; i < values.length; i++) wav[44 + i] = values[i] + 128;
  return new Blob([wav], { type: 'audio/wav' });
}

function sanitizeVarName(name) {
  let s = (name || '').trim().replace(/[^A-Za-z0-9_]/g, '_');
  if (!s) s = 'engineSound';
  if (/^[0-9]/.test(s)) s = '_' + s;
  return s;
}

function downloadBlob(blob, filename) {
  const a = document.createElement('a');
  a.href = URL.createObjectURL(blob);
  a.download = filename;
  document.body.appendChild(a);
  a.click();
  setTimeout(() => {
    URL.revokeObjectURL(a.href);
    a.remove();
  }, 200);
}

function floatToPcm8(floatData, normalize) {
  if (!floatData || !floatData.length) return new Int8Array(0);
  let peak = 1;
  if (normalize) {
    peak = 0;
    for (let i = 0; i < floatData.length; i++) peak = Math.max(peak, Math.abs(floatData[i]));
  }
  const out = new Int8Array(floatData.length);
  for (let i = 0; i < floatData.length; i++) {
    const v = Math.max(-1, Math.min(1, floatData[i] / peak));
    out[i] = Math.max(-128, Math.min(127, Math.round(v * 127)));
  }
  return out;
}

function resampleLinear(floatData, fromRate, toRate) {
  if (fromRate === toRate) return floatData;
  const ratio = fromRate / toRate;
  const outLen = Math.max(1, Math.floor(floatData.length / ratio));
  const out = new Float32Array(outLen);
  for (let i = 0; i < outLen; i++) {
    const pos = i * ratio;
    const idx = Math.floor(pos);
    const frac = pos - idx;
    const a = floatData[idx] || 0;
    const b = floatData[Math.min(floatData.length - 1, idx + 1)] || a;
    out[i] = a + (b - a) * frac;
  }
  return out;
}

function crossfadeLoop(pcm8arr, fadeSamples) {
  // Crossfade the end into the start so looping is seamless
  if (!fadeSamples || fadeSamples < 2 || pcm8arr.length < fadeSamples * 2) return pcm8arr;
  const out = new Int8Array(pcm8arr.length - fadeSamples);
  // Copy the middle part
  for (let i = fadeSamples; i < pcm8arr.length - fadeSamples; i++) {
    out[i] = pcm8arr[i];
  }
  // Crossfade zone: blend end into start
  for (let i = 0; i < fadeSamples; i++) {
    const t = i / fadeSamples; // 0→1
    const fromEnd = pcm8arr[pcm8arr.length - fadeSamples + i];
    const fromStart = pcm8arr[i];
    out[i] = Math.max(-128, Math.min(127, Math.round(fromEnd * (1 - t) + fromStart * t)));
  }
  return out;
}

// RMS-envelope compressor: evens out loud & quiet spots
// amount: 0-1 (0=bypass, 1=full compression)
function compressPcm8(pcm8arr, amount) {
  if (!amount || amount <= 0 || pcm8arr.length < 100) return pcm8arr;
  const len = pcm8arr.length;
  // Window size ~5ms at 22050 → ~110 samples, but scale with length
  const winSize = Math.max(32, Math.min(512, Math.floor(len / 50)));
  const half = Math.floor(winSize / 2);

  // Compute RMS envelope
  const env = new Float32Array(len);
  let sumSq = 0;
  for (let i = 0; i < Math.min(winSize, len); i++) sumSq += (pcm8arr[i] / 128.0) ** 2;
  for (let i = 0; i < len; i++) {
    // Slide window
    const addIdx = i + half;
    const remIdx = i - half - 1;
    if (addIdx < len) sumSq += (pcm8arr[addIdx] / 128.0) ** 2;
    if (remIdx >= 0) sumSq -= (pcm8arr[remIdx] / 128.0) ** 2;
    if (sumSq < 0) sumSq = 0; // float safety
    const cnt = Math.min(addIdx + 1, len) - Math.max(remIdx + 1, 0);
    env[i] = Math.sqrt(sumSq / cnt);
  }

  // Target level = overall RMS
  let totalSq = 0;
  for (let i = 0; i < len; i++) totalSq += (pcm8arr[i] / 128.0) ** 2;
  const targetRms = Math.sqrt(totalSq / len);
  if (targetRms < 0.001) return pcm8arr;

  // Apply gain correction: where envelope is loud, reduce; where quiet, boost
  const out = new Int8Array(len);
  for (let i = 0; i < len; i++) {
    const localRms = Math.max(env[i], 0.005); // floor to avoid divide-by-huge
    const gain = targetRms / localRms;
    // Limit gain to avoid extreme boost of silence
    const clampedGain = Math.min(gain, 4.0);
    // Blend between original (1.0) and compressed (clampedGain) based on amount
    const finalGain = 1.0 + (clampedGain - 1.0) * amount;
    const sample = pcm8arr[i] * finalGain;
    out[i] = Math.max(-128, Math.min(127, Math.round(sample)));
  }
  return out;
}

async function convertWavToHeader() {
  const f = document.getElementById('convWavFile');
  const outRateEl = document.getElementById('convOutRate');
  const outArea = document.getElementById('convHeaderOut');
  const varNameEl = document.getElementById('convVarName');
  const normalizeEl = document.getElementById('convNormalize');
  const speedEl = document.getElementById('convSpeed');

  if (!f || !f.files || !f.files.length) {
    status('Choose a WAV file first', 'warn');
    return;
  }

  const file = f.files[0];
  const targetRate = parseInt(outRateEl.value, 10) || 22050;
  const speed = parseFloat(speedEl ? speedEl.value : '1') || 1.0;
  const varName = sanitizeVarName(varNameEl.value);
  const normalize = !!(normalizeEl && normalizeEl.checked);

  const buf = await file.arrayBuffer();
  const ac = new (window.AudioContext || window.webkitAudioContext)();
  const audio = await ac.decodeAudioData(buf.slice(0));
  let mono = audio.numberOfChannels > 1 ? (() => {
    const left = audio.getChannelData(0);
    const right = audio.getChannelData(1);
    const m = new Float32Array(audio.length);
    for (let i = 0; i < m.length; i++) m[i] = (left[i] + right[i]) * 0.5;
    return m;
  })() : audio.getChannelData(0);

  // Show duration info
  const durEl = document.getElementById('convDuration');
  if (durEl) durEl.textContent = 'Source: ' + (audio.length / audio.sampleRate).toFixed(2) + 's @ ' + audio.sampleRate + ' Hz';

  // Auto trim
  const autoTrimEl = document.getElementById('convAutoTrim');
  const autoTrimMaxEl = document.getElementById('convAutoTrimMax');
  const autoTrimOn = autoTrimEl && autoTrimEl.checked;
  const autoTrimMax = parseFloat(autoTrimMaxEl ? autoTrimMaxEl.value : '2') || 2;
  let wasAutoTrimmed = false;
  if (autoTrimOn) {
    const maxSamples = Math.floor(autoTrimMax * audio.sampleRate);
    if (mono.length > maxSamples) {
      mono = mono.slice(0, maxSamples);
      wasAutoTrimmed = true;
    }
  }

  // Speed control: slower = more samples (higher effective rate), faster = fewer samples
  // E.g. 0.5x speed → resample to 2x rate → double the samples → plays half speed on device
  const effectiveRate = targetRate / speed;
  const resampled = resampleLinear(mono, audio.sampleRate, effectiveRate);
  let pcm8 = floatToPcm8(resampled, normalize);

  // Apply loop crossfade to eliminate gap at loop point
  const loopFadeEl = document.getElementById('convLoopFade');
  if (loopFadeEl && loopFadeEl.checked && pcm8.length > 200) {
    const fadeSamples = Math.min(Math.floor(pcm8.length * 0.05), 500);
    pcm8 = crossfadeLoop(pcm8, fadeSamples);
  }

  const lines = [];
  lines.push('// Generated by integrated Sound Forge');
  lines.push('#pragma once');
  lines.push('const unsigned int ' + varName + '_sampleRate = ' + targetRate + ';');
  lines.push('const unsigned int ' + varName + '_sampleCount = ' + pcm8.length + ';');
  lines.push('const signed char ' + varName + '[] = {');
  let row = '  ';
  for (let i = 0; i < pcm8.length; i++) {
    row += pcm8[i].toString();
    if (i !== pcm8.length - 1) row += ', ';
    if ((i + 1) % 20 === 0 && i !== pcm8.length - 1) {
      lines.push(row);
      row = '  ';
    }
  }
  if (row.trim()) lines.push(row);
  lines.push('};');

  outArea.value = lines.join('\n');
  const speedLabel = speed !== 1 ? ' @ ' + speed + 'x speed' : '';
  const sizeKB = Math.round(pcm8.length / 1024);
  const trimLabel = wasAutoTrimmed ? ', auto-trimmed to ' + autoTrimMax + 's' : '';
  const durationSec = (pcm8.length / targetRate).toFixed(2);
  let sizeWarn = '';
  if (sizeKB > 500) sizeWarn = ' ⚠️ LARGE FILE — may cause build failure! Try trimming shorter.';
  else if (sizeKB > 200) sizeWarn = ' ⚠️ Large — consider trimming shorter.';
  status('Converted: ' + file.name + speedLabel + trimLabel + ' — ' + durationSec + 's, ' + pcm8.length + ' samples, ~' + sizeKB + ' KB' + sizeWarn, sizeKB > 500 ? 'warn' : '');
}

function downloadHeaderText() {
  const outArea = document.getElementById('convHeaderOut');
  const varNameEl = document.getElementById('convVarName');
  const text = (outArea && outArea.value) ? outArea.value : '';
  if (!text.trim()) {
    status('No header text to download', 'warn');
    return;
  }
  const varName = sanitizeVarName(varNameEl.value);
  downloadBlob(new Blob([text], {type: 'text/plain'}), varName + '.h');
}

async function installHeaderToSrc() {
  const outArea = document.getElementById('convHeaderOut');
  const varNameEl = document.getElementById('convVarName');
  const catSelect = document.getElementById('convSoundCategory');
  const text = (outArea && outArea.value) ? outArea.value : '';
  if (!text.trim()) {
    status('Convert a WAV first', 'warn');
    return;
  }
  const category = catSelect ? catSelect.value : '';
  if (!category) {
    status('Pick a sound category (Install as) so the sound shows up in the right dropdown', 'warn');
    return;
  }
  const varName = sanitizeVarName(varNameEl.value);
  const filename = varName + '.h';
  try {
    const r = await fetch('/install_header', {
      method: 'POST',
      headers: {'Content-Type': 'application/json'},
      body: JSON.stringify({filename, text, category})
    });
    const j = await r.json();
    if (j.ok) {
      const extra = j.added_to_vehicle ? ' and added to vehicle dropdown' : '';
      status('\u2714 Installed ' + filename + ' \u2192 sounds/' + extra + ' (reloading...)', '');
      setTimeout(() => location.reload(), 800);
    } else {
      status('Install failed: ' + (j.error || '?'), 'bad');
    }
  } catch(e) {
    status('Install error: ' + e, 'bad');
  }
}

const _catFilenames = {};  // category key -> suggested filename

async function loadSoundCategories() {
  const sel = document.getElementById('convSoundCategory');
  if (!sel) return;
  try {
    const r = await fetch('/sound_categories');
    const j = await r.json();
    sel.innerHTML = '<option value="">-- pick where this sound goes --</option>';
    if (j.categories && j.categories.length) {
      j.categories.forEach(c => {
        const o = document.createElement('option');
        o.value = c.key;
        o.textContent = c.title;
        if (c.filename) _catFilenames[c.key] = c.filename;
        sel.appendChild(o);
      });
    } else {
      sel.innerHTML = '<option value="">No sound categories found</option>';
    }
  } catch(e) {
    sel.innerHTML = '<option value="">Error loading categories</option>';
  }
}

function onCategoryChanged(sel) {
  const key = sel.value;
  const varEl = document.getElementById('convVarName');
  if (!varEl) return;
  if (key && _catFilenames[key]) {
    varEl.value = _catFilenames[key];
  } else {
    varEl.value = '';
  }
}

function loadHeaderFileIntoEditor(event) {
  const file = event.target.files && event.target.files[0];
  if (!file) return;
  file.text().then(txt => {
    const box = document.getElementById('convHeaderIn');
    if (box) box.value = txt;
  });
}

function convertHeaderToWav() {
  const box = document.getElementById('convHeaderIn');
  const audio = document.getElementById('convAudioPreview');
  const info = document.getElementById('convInfo');
  const txt = (box && box.value) ? box.value : '';
  if (!txt.trim()) {
    if (info) info.textContent = 'Paste or load a header first';
    return;
  }
  try {
    convertedWavBlob = previewHeaderToWavBlob(txt);
    const url = URL.createObjectURL(convertedWavBlob);
    if (audio) {
      audio.src = url;
      audio.play().catch(() => {});
    }
    if (info) info.textContent = 'Converted header to WAV preview';
  } catch (e) {
    if (info) info.textContent = 'Conversion error: ' + e;
  }
}

function downloadConvertedWav() {
  if (!convertedWavBlob) {
    status('No WAV is ready yet. Convert header first.', 'warn');
    return;
  }
  downloadBlob(convertedWavBlob, 'converted.wav');
}

async function previewConvertedSound(event) {
  const file = event.target.files && event.target.files[0];
  if (!file) return;
  const audio = document.getElementById('previewAudio');
  const info = document.getElementById('previewInfo');
  if (!audio || !info) return;
  try {
    const lower = file.name.toLowerCase();
    let url = null;
    if (lower.endsWith('.wav')) {
      url = URL.createObjectURL(file);
      info.textContent = 'Previewing WAV: ' + file.name;
    } else if (lower.endsWith('.h')) {
      const text = await file.text();
      const blob = previewHeaderToWavBlob(text);
      url = URL.createObjectURL(blob);
      info.textContent = 'Previewing converted header: ' + file.name;
    } else {
      info.textContent = 'Unsupported file type. Use .wav or .h';
      return;
    }
    audio.src = url;
    audio.play().catch(() => {});
  } catch (err) {
    info.textContent = 'Preview error: ' + err;
  }
}

function collectVehicleDetailDraft() {
  const details = document.getElementById('selectedVehicleDetails');
  if (!details) return null;
  const vehicle = details.getAttribute('data-vehicle-file') || '';
  const form = details.querySelector('form[data-file]');
  if (!vehicle || !form) return null;
  const draft = {};
  form.querySelectorAll('input,select,textarea').forEach(el => {
    if (!el.name) return;
    if (el.type === 'checkbox') draft[el.name] = !!el.checked;
    else draft[el.name] = el.value;
  });
  return {vehicle, draft};
}

function applyVehicleDraft(vehicle) {
  const details = document.getElementById('selectedVehicleDetails');
  if (!details) return;
  const form = details.querySelector('form[data-file]');
  if (!form) return;
  const draft = vehicleDrafts[vehicle];
  if (!draft) return;
  form.querySelectorAll('input,select,textarea').forEach(el => {
    if (!el.name || !(el.name in draft)) return;
    if (el.type === 'checkbox') el.checked = !!draft[el.name];
    else el.value = draft[el.name];
  });
}

async function handleVehicleSelection(sel) {
  if (!sel || !sel.value) return;
  const snap = collectVehicleDetailDraft();
  if (snap && snap.vehicle) {
    vehicleDrafts[snap.vehicle] = snap.draft;
  }

  try {
    const sv = await fetch('/set_vehicle', {
      method: 'POST',
      headers: {'Content-Type': 'application/json'},
      body: JSON.stringify({vehicle: sel.value})
    });
    const j1 = await sv.json();
    if (!j1.ok) throw new Error(j1.error || 'Vehicle switch failed');

    const sec = await fetch('/vehicle_section?vehicle=' + encodeURIComponent(sel.value));
    const j2 = await sec.json();
    if (!j2.ok) throw new Error(j2.error || 'Vehicle section fetch failed');

    const oldDetails = document.getElementById('selectedVehicleDetails');
    if (!oldDetails) throw new Error('No selected vehicle section found');
    const tmp = document.createElement('div');
    tmp.innerHTML = j2.html;
    const newDetails = tmp.firstElementChild;
    if (!newDetails) throw new Error('Invalid vehicle section response');
    oldDetails.replaceWith(newDetails);

    applyVehicleDraft(sel.value);
    setAdvancedSoundVisibility(showAdvancedSound);
    updateVehicleNameDisplay();
    // Persist immediately so page refresh always loads the right vehicle
    localStorage.setItem('lastVehicle', sel.value);
    markDirty();
    status('Live vehicle loaded: ' + sel.value, 'warn');
  } catch (e) {
    status('Vehicle live switch failed: ' + e, 'bad');
  }
}

async function loadPorts() {
  const sel = document.getElementById('portSelect');
  const old = sel.value;
  sel.innerHTML = '';
  try {
    const r = await fetch('/ports');
    const j = await r.json();
    const ports = j.ports || [];

    if (!ports.length) {
      const opt = document.createElement('option');
      opt.value = '';
      opt.textContent = 'No ports found';
      sel.appendChild(opt);
      return;
    }

    ports.forEach(p => {
      const opt = document.createElement('option');
      opt.value = p;
      opt.textContent = p;
      if (p === old || p === j.connected) {
        opt.selected = true;
      }
      sel.appendChild(opt);
    });

    if (j.connected) {
      connectedPort = j.connected;
    }
    refreshStatusLabel();
  } catch (e) {
    status('Connection failed: ' + e, 'bad');
  }
}

async function connectBoard() {
  const sel = document.getElementById('portSelect');
  const port = sel.value;
  if (!port) {
    status('Select a board port first', 'bad');
    return;
  }

  try {
    const r = await fetch('/connect', {
      method: 'POST',
      headers: {'Content-Type': 'application/json'},
      body: JSON.stringify({port: port})
    });
    const j = await r.json();
    if (j.ok) {
      connectedPort = j.port;
      refreshStatusLabel();
    } else {
      status('Connection failed: ' + (j.error || 'unknown'), 'bad');
    }
  } catch (e) {
    status('Connection failed: ' + e, 'bad');
  }
}

function collectForms() {
  const files = {};
  document.querySelectorAll('form[data-file]').forEach(form => {
    const file = form.dataset.file;
    const data = {};

    const vehicleSel = form.querySelector('select[name="__vehicle__"]');
    if (vehicleSel) {
      data['__vehicle__'] = vehicleSel.value;
    }

    form.querySelectorAll('input,select').forEach(el => {
      if (el.name === '__vehicle__') return;
      if (el.name.endsWith('__enabled')) return;

      if (el.type === 'checkbox') {
        if (el.dataset.vartype === 'bool') {
          data[el.name] = {kind: 'bool_var', value: el.checked ? 'true' : 'false'};
        } else {
          data[el.name] = {kind: 'flag', enabled: el.checked};
        }
      } else if (el.type === 'text' || el.type === 'range' || el.tagName === 'SELECT') {
        if (el.name.startsWith('__sound__')) {
          data[el.name] = {kind: 'sound_choice', value: el.value};
          return;
        }
        const en = form.querySelector('input[name="' + el.name + '__enabled"]');
        if (en) {
          data[el.name] = {kind: 'define_val', enabled: en.checked, value: el.value};
        } else {
          data[el.name] = {kind: 'text_var', value: el.value};
        }
      }
    });

    files[file] = data;
  });
  return files;
}

async function saveAll() {
  try {
    const r = await fetch('/save', {
      method: 'POST',
      headers: {'Content-Type': 'application/json'},
      body: JSON.stringify(collectForms())
    });
    const j = await r.json();
    if (j.ok) {
      dirty = false;
      if (j.fixes && j.fixes.length) {
        status('Saved (auto-fixed: ' + j.fixes.join(', ') + ')', 'warn');
      } else if (connectedPort) {
        status('Connected to ' + connectedPort, '');
      } else {
        status('Saved', '');
      }
      // Remember which vehicle was used last
      const vehicle = getSelectedVehicleFile();
      if (vehicle) localStorage.setItem('lastVehicle', vehicle);
    } else {
      status('Save failed: ' + (j.error || 'unknown'), 'bad');
    }
  } catch (e) {
    status('Save failed: ' + e, 'bad');
  }
}

async function autoLoadLastSession() {
  const lastVehicle = localStorage.getItem('lastVehicle');
  if (!lastVehicle) return;
  const vehicleSel = document.querySelector('form[data-file="1_Vehicle.h"] select[name="__vehicle__"]');
  if (!vehicleSel) return;
  const opts = Array.from(vehicleSel.options);
  if (!opts.some(o => o.value === lastVehicle)) return;
  if (vehicleSel.value !== lastVehicle) {
    vehicleSel.value = lastVehicle;
    await handleVehicleSelection(vehicleSel);
  }
}

function closeLog() {
  document.getElementById('log').className = '';
}

function showBuildFailModal() {
  document.getElementById('buildFailModal').classList.add('visible');
}
function closeBuildFailModal() {
  document.getElementById('buildFailModal').classList.remove('visible');
}

function setBuildLight(state) {
  const light = document.getElementById('buildLight');
  if (!light) return;
  if (state === 'ok') { light.style.background = '#1ce8b5'; light.style.borderColor = '#0fa'; light.style.boxShadow = '0 0 8px #1ce8b5'; light.title = 'Build succeeded'; }
  else if (state === 'fail') { light.style.background = '#f87171'; light.style.borderColor = '#f44'; light.style.boxShadow = '0 0 8px #f87171'; light.title = 'Build failed'; }
  else { light.style.background = '#333'; light.style.borderColor = '#555'; light.style.boxShadow = 'none'; light.title = 'Build status'; }
}

async function runCmd(cmd) {
  if (dirty) {
    await saveAll();
  }

  if (cmd === 'flash' && !connectedPort) {
    status('Connect to board before flashing', 'bad');
    return;
  }

  // Reset build light on new build/flash
  setBuildLight('none');

  const log = document.getElementById('log');
  const body = document.getElementById('log-body');
  const title = document.getElementById('log-title');

  log.className = 'open';
  title.textContent = (cmd === 'flash') ? 'Flash Output' : 'Build Output';
  body.textContent = 'Starting ' + cmd + '...\n';

  try {
    const resp = await fetch('/run', {
      method: 'POST',
      headers: {'Content-Type': 'application/json'},
      body: JSON.stringify({cmd: cmd, vehicle: document.getElementById('vehicleSelect')?.value || ''})
    });

    if (!resp.body) {
      body.textContent += '\nNo output stream returned.\n';
      setBuildLight('fail');
      return;
    }

    let fullOutput = '';
    const reader = resp.body.getReader();
    const decoder = new TextDecoder();
    while (true) {
      const x = await reader.read();
      if (x.done) break;
      const chunk = decoder.decode(x.value);
      fullOutput += chunk;
      body.textContent += chunk;
      body.scrollTop = body.scrollHeight;
    }

    // Parse exit code from "--- DONE (exit N) ---"
    const exitMatch = fullOutput.match(/--- DONE \(exit (\d+)\) ---/);
    if (exitMatch) {
      const exitCode = parseInt(exitMatch[1]);
      setBuildLight(exitCode === 0 ? 'ok' : 'fail');
      if (exitCode === 0) {
        status(cmd === 'flash' ? 'Flash complete!' : 'Build succeeded!', '');
        if (cmd === 'flash') { setTimeout(closeLog, 2500); }
      } else {
        status(cmd === 'flash' ? 'Flash failed (exit ' + exitCode + ')' : 'Build failed (exit ' + exitCode + ')', 'bad');
        showBuildFailModal();
      }
    } else {
      setBuildLight('fail');
      showBuildFailModal();
    }
  } catch (e) {
    body.textContent += '\nError: ' + e + '\n';
    setBuildLight('fail');
    showBuildFailModal();
  }
}

async function openArduinoIDE() {
  if (dirty) {
    await saveAll();
  }

  try {
    const r = await fetch('/open_arduino_ide', {
      method: 'POST',
      headers: {'Content-Type': 'application/json'},
      body: JSON.stringify({})
    });
    const j = await r.json();
    if (j.ok) {
      status(j.message || 'Arduino IDE opened', '');
    } else {
      status('Arduino IDE open failed: ' + (j.error || 'unknown'), 'bad');
    }
  } catch (e) {
    status('Arduino IDE open failed: ' + e, 'bad');
  }
}

// ── Volume slider ──────────────────────────────────────────
function onVolumeSliderChange(val) {
  document.getElementById('masterVolLabel').textContent = val + '%';
}
async function saveVolumeSlider(val) {
  try {
    const r = await fetch('/set_volume', {
      method: 'POST',
      headers: {'Content-Type': 'application/json'},
      body: JSON.stringify({volume: parseInt(val)})
    });
    const j = await r.json();
    if (j.ok) status('Volume set to ' + val + '%', '');
    else status('Volume save failed: ' + (j.error || '?'), 'bad');
  } catch(e) { status('Volume save error: ' + e, 'bad'); }
}
async function loadVolume() {
  try {
    const r = await fetch('/get_volume');
    const j = await r.json();
    if (j.ok) {
      const sl = document.getElementById('masterVolSlider');
      const lb = document.getElementById('masterVolLabel');
      if (sl) sl.value = j.volume;
      if (lb) lb.textContent = j.volume + '%';
      const cb = document.getElementById('volPotOverride');
      if (cb) cb.checked = !!j.potOverride;
    }
  } catch(e) {}
}
async function saveVolPotOverride(checked) {
  try {
    const r = await fetch('/set_vol_pot_override', {
      method: 'POST',
      headers: {'Content-Type': 'application/json'},
      body: JSON.stringify({enabled: checked})
    });
    const j = await r.json();
    if (j.ok) status('Vol pot override ' + (checked ? 'ON' : 'OFF'), '');
    else status('Override save failed: ' + (j.error || '?'), 'bad');
  } catch(e) { status('Override save error: ' + e, 'bad'); }
}

// ── Custom sounds management ──────────────────────────────
async function loadCustomSounds() {
  const el = document.getElementById('customSoundsList');
  if (!el) return;
  try {
    const r = await fetch('/custom_sounds');
    const j = await r.json();
    if (!j.ok || !j.sounds || !j.sounds.length) {
      el.innerHTML = '<span style="color:#666">No custom sounds installed yet.</span>';
      return;
    }
    el.innerHTML = j.sounds.map(s =>
      '<div style="display:flex;align-items:center;gap:8px;margin-bottom:4px;padding:3px 6px;background:#111;border-radius:4px">' +
        '<span style="flex:1;color:#93c5fd">' + escHtml(s.file) + '</span>' +
        '<span style="color:#888;font-size:11px">' + escHtml(s.category) + '</span>' +
        '<button onclick="deleteCustomSound(\'' + escHtml(s.file) + '\')" ' +
          'style="background:#c62828;color:#fff;border:none;padding:2px 8px;border-radius:3px;cursor:pointer;font-size:11px" ' +
          'title="Delete this sound file and remove from vehicle">✕ Delete</button>' +
      '</div>'
    ).join('');
  } catch(e) {
    el.innerHTML = '<span style="color:#f44">Error loading sounds</span>';
  }
}
function escHtml(s) { return String(s).replace(/&/g,'&amp;').replace(/</g,'&lt;').replace(/>/g,'&gt;').replace(/"/g,'&quot;').replace(/'/g,'&#39;'); }

async function deleteCustomSound(filename) {
  if (!confirm('Delete "' + filename + '" and remove it from the vehicle file?')) return;
  try {
    const r = await fetch('/delete_sound', {
      method: 'POST',
      headers: {'Content-Type': 'application/json'},
      body: JSON.stringify({filename: filename})
    });
    const j = await r.json();
    if (j.ok) {
      status('Deleted ' + filename + (j.removed_from_vehicle ? ' and removed from vehicle' : ''), '');
      loadCustomSounds();
      loadSoundCategories();
    } else {
      status('Delete failed: ' + (j.error || '?'), 'bad');
    }
  } catch(e) { status('Delete error: ' + e, 'bad'); }
}

async function sbDeleteCustom(filename) {
  if (!confirm('Delete custom sound "' + filename + '"?')) return;
  try {
    const r = await fetch('/delete_sound', {
      method: 'POST',
      headers: {'Content-Type': 'application/json'},
      body: JSON.stringify({filename: filename})
    });
    const j = await r.json();
    if (j.ok) {
      status('Deleted ' + filename, '');
      _sbCustomFiles.delete(filename);
      _sbSounds = _sbSounds.filter(s => s.file !== filename);
      renderSoundBrowser();
      loadCustomSounds();
    } else {
      status('Delete failed: ' + (j.error || '?'), 'bad');
    }
  } catch(e) { status('Delete error: ' + e, 'bad'); }
}

// ===== Sound Browser =====
let _sbSounds = [];
let _sbCustomFiles = new Set();
let _sbAudioCtx = null;
let _sbSource = null;
let _sbGain = null;
let _sbBuffer = null;
let _sbPlaying = false;
let _sbCurrentFile = '';
let _sbLoopBuf = null;       // crossfaded loop AudioBuffer used for preview
let _sbLoopStartTime = 0;    // ctx.currentTime when loop source started
let _sbSwapTimer = null;      // debounce timer for live slider updates
let _sbHighPass = null;       // BiquadFilterNode highpass
let _sbLowPass = null;        // BiquadFilterNode lowpass

function _getAudioCtx() {
  if (!_sbAudioCtx) _sbAudioCtx = new (window.AudioContext || window.webkitAudioContext)();
  return _sbAudioCtx;
}

function sbUpdateFilters() {
  const ctx = _getAudioCtx();
  if (_sbHighPass) {
    const hp = parseInt(document.getElementById('sbHighPass').value) || 0;
    const hpVal = hp > 0 ? hp : 10;
    _sbHighPass.frequency.setValueAtTime(_sbHighPass.frequency.value, ctx.currentTime);
    _sbHighPass.frequency.linearRampToValueAtTime(hpVal, ctx.currentTime + 0.1);
  }
  if (_sbLowPass) {
    const lp = parseInt(document.getElementById('sbLowPass').value) || 11025;
    _sbLowPass.frequency.setValueAtTime(_sbLowPass.frequency.value, ctx.currentTime);
    _sbLowPass.frequency.linearRampToValueAtTime(lp, ctx.currentTime + 0.1);
  }
}

function _sbConnectChain(source, ctx) {
  // source → highpass → lowpass → gain → destination
  const hp = parseInt(document.getElementById('sbHighPass').value) || 0;
  const lp = parseInt(document.getElementById('sbLowPass').value) || 11025;
  _sbHighPass = ctx.createBiquadFilter();
  _sbHighPass.type = 'highpass';
  _sbHighPass.frequency.value = hp > 0 ? hp : 10;
  _sbHighPass.Q.value = 0.7;
  _sbLowPass = ctx.createBiquadFilter();
  _sbLowPass.type = 'lowpass';
  _sbLowPass.frequency.value = lp;
  _sbLowPass.Q.value = 0.7;
  const volSlider = document.getElementById('sbVolSlider');
  _sbGain = ctx.createGain();
  _sbGain.gain.value = (volSlider ? parseInt(volSlider.value) : 100) / 100;
  source.connect(_sbHighPass);
  _sbHighPass.connect(_sbLowPass);
  _sbLowPass.connect(_sbGain);
  _sbGain.connect(ctx.destination);
  return _sbGain;
}

async function previewSoundFromDropdown(btn) {
  const sel = btn.previousElementSibling;
  if (!sel || !sel.value) return;
  const filename = sel.value;
  sbStop();
  btn.textContent = '...';
  try {
    const r = await fetch('/sound_pcm/' + encodeURIComponent(filename));
    const j = await r.json();
    if (!j.ok) { btn.innerHTML = '&#9654;'; status('Cannot preview: ' + (j.error || '?'), 'warn'); return; }
    const ctx = _getAudioCtx();
    const buf = ctx.createBuffer(1, j.samples.length, j.sampleRate);
    const ch = buf.getChannelData(0);
    for (let i = 0; i < j.samples.length; i++) ch[i] = j.samples[i] / 128.0;
    _sbBuffer = buf;
    _sbCurrentFile = filename;
    _sbSource = ctx.createBufferSource();
    _sbSource.buffer = buf;
    _sbSource.loop = true;
    _sbGain = ctx.createGain();
    _sbGain.gain.value = 1.0;
    _sbSource.connect(_sbGain);
    _sbGain.connect(ctx.destination);
    _sbSource.onended = function() { _sbPlaying = false; btn.innerHTML = '&#9654;'; };
    _sbSource.start(0);
    _sbPlaying = true;
    btn.innerHTML = '&#9724;';
    btn.onclick = function() { sbStop(); btn.innerHTML = '&#9654;'; btn.onclick = function() { previewSoundFromDropdown(btn); }; };
  } catch(e) { btn.innerHTML = '&#9654;'; status('Preview error: ' + e, 'warn'); }
}

async function loadSoundBrowser() {
  if (_sbSounds.length) { renderSoundBrowser(); return; }
  const list = document.getElementById('sbList');
  if (list) list.innerHTML = '<div style="padding:12px;color:#666">Loading sounds...</div>';
  // Load custom sounds set for delete buttons
  try {
    const cr2 = await fetch('/custom_sounds');
    const cj2 = await cr2.json();
    _sbCustomFiles = new Set((cj2.sounds || []).map(s => s.file));
  } catch(e) { _sbCustomFiles = new Set(); }
  // Load install categories for the Sound Browser dropdown
  try {
    const catSel = document.getElementById('sbInstallCategory');
    if (catSel && catSel.options.length <= 1) {
      const cr = await fetch('/sound_categories');
      const cj = await cr.json();
      catSel.innerHTML = '<option value="">-- pick where this sound goes --</option>';
      if (cj.categories) cj.categories.forEach(c => {
        const o = document.createElement('option');
        o.value = c.key; o.textContent = c.title;
        if (c.filename) o.dataset.filename = c.filename;
        catSel.appendChild(o);
      });
    }
  } catch(e) {}
  try {
    const r = await fetch('/all_sounds');
    const j = await r.json();
    if (j.ok && j.sounds) {
      _sbSounds = j.sounds;
      renderSoundBrowser();
    }
  } catch(e) { if (list) list.innerHTML = '<div style="padding:12px;color:red">Error: ' + e + '</div>'; }
}

function filterSoundBrowser() { renderSoundBrowser(); }

function renderSoundBrowser() {
  const list = document.getElementById('sbList');
  const catFilter = document.getElementById('sbCatFilter');
  const searchEl = document.getElementById('sbSearch');
  const countEl = document.getElementById('sbCount');
  if (!list) return;
  const cat = catFilter ? catFilter.value : '';
  const q = (searchEl ? searchEl.value : '').toLowerCase();
  const filtered = _sbSounds.filter(s => {
    if (cat && s.category !== cat) return false;
    if (q && !s.label.toLowerCase().includes(q) && !s.file.toLowerCase().includes(q)) return false;
    return true;
  });
  if (countEl) countEl.textContent = filtered.length + ' / ' + _sbSounds.length + ' sounds';
  if (!filtered.length) {
    list.innerHTML = '<div style="padding:12px;color:#666">No sounds match filter</div>';
    return;
  }
  const catColors = {idle:'#4ade80',rev:'#f87171',start:'#fbbf24',knock:'#f97316',jakebrake:'#c084fc',
    horn:'#60a5fa',siren:'#f472b6',airbrake:'#94a3b8',turbo:'#22d3ee',wastegate:'#a78bfa',
    trackrattle:'#a3e635',other:'#666'};
  let html = '<table style="width:100%;table-layout:fixed;border-collapse:collapse;font-size:12px"><colgroup><col style="width:auto"><col style="width:75px"><col style="width:60px"></colgroup>';
  filtered.forEach(s => {
    const color = catColors[s.category] || '#888';
    const playing = s.file === _sbCurrentFile;
    const hl = playing ? 'background:#1a2a1a;' : '';
    html += '<tr style="border-bottom:1px solid #222;cursor:pointer;' + hl + '" onclick="sbLoadSound(\'' + s.file.replace(/'/g, "\\'") + '\')" title="Click to load: ' + s.file + '">';
    html += '<td style="padding:6px 8px;color:#eee;white-space:nowrap;overflow:hidden;text-overflow:ellipsis">' + (playing ? '&#9654; ' : '') + s.label + '</td>';
    html += '<td style="padding:6px 4px;text-align:center"><span style="color:' + color + ';font-size:10px;background:#111;padding:1px 6px;border-radius:8px;white-space:nowrap">' + s.category + '</span></td>';
    html += '<td style="padding:6px 4px;text-align:right;white-space:nowrap">';
    html += '<button class="btn-cyan" style="font-size:11px;padding:2px 8px" onclick="event.stopPropagation();sbLoadAndPlay(\'' + s.file.replace(/'/g, "\\'") + '\')">&#9654;</button>';
    if (_sbCustomFiles.has(s.file)) {
      html += ' <button style="background:#c62828;color:#fff;border:none;padding:2px 6px;border-radius:3px;cursor:pointer;font-size:10px" onclick="event.stopPropagation();sbDeleteCustom(\'' + s.file.replace(/'/g, "\\'") + '\')" title="Delete custom sound">&#10005;</button>';
    }
    html += '</td>';
    html += '</tr>';
  });
  html += '</table>';
  list.innerHTML = html;
}

async function sbLoadSound(filename) {
  sbStop();
  _sbCurrentFile = filename;
  const info = document.getElementById('sbSoundInfo');
  const nameEl = document.getElementById('sbNowPlaying');
  if (nameEl) nameEl.textContent = 'Loading ' + filename + '...';
  if (info) info.textContent = '';
  try {
    const r = await fetch('/sound_pcm/' + encodeURIComponent(filename));
    const j = await r.json();
    if (!j.ok) { if (nameEl) nameEl.textContent = 'Error: ' + (j.error || '?'); return; }
    // Convert PCM int8 array to AudioBuffer
    const ctx = _getAudioCtx();
    const buf = ctx.createBuffer(1, j.samples.length, j.sampleRate);
    const ch = buf.getChannelData(0);
    for (let i = 0; i < j.samples.length; i++) ch[i] = j.samples[i] / 128.0;
    _sbBuffer = buf;
    _sbRawSamples = j.samples;
    _sbRawRate = j.sampleRate;
    const dur = (j.samples.length / j.sampleRate).toFixed(3);
    if (nameEl) nameEl.textContent = filename.replace('.h', '');
    if (info) info.textContent = j.sampleRate + ' Hz, ' + j.samples.length + ' samples, ' + dur + 's';
    // Reset loop sliders to full range
    const startSlider = document.getElementById('sbLoopStart');
    const endSlider = document.getElementById('sbLoopEnd');
    if (startSlider) startSlider.value = 0;
    if (endSlider) endSlider.value = 1;
    sbUpdateLoopPoints();
    renderSoundBrowser();
  } catch(e) { if (nameEl) nameEl.textContent = 'Error loading: ' + e; }
}

async function sbLoadAndPlay(filename) {
  await sbLoadSound(filename);
  sbPlay();
}

function sbPlay() {
  if (!_sbBuffer) return;
  sbStop();
  const ctx = _getAudioCtx();
  const dur = _sbBuffer.duration;
  const startSlider = document.getElementById('sbLoopStart');
  const endSlider = document.getElementById('sbLoopEnd');
  const ls = parseFloat(startSlider ? startSlider.value : '0') * dur;
  const le = parseFloat(endSlider ? endSlider.value : '1') * dur;
  const doLoop = !!(document.getElementById('sbLoop') && document.getElementById('sbLoop').checked);

  // Always use a pre-built preview buffer for looping (with at least a tiny
  // micro-crossfade to eliminate the click/pause at loop boundaries)
  if (doLoop) {
    const previewBuf = _sbBuildPreviewBuf(ctx);
    if (previewBuf) {
      _sbLoopBuf = previewBuf;
      _sbSource = ctx.createBufferSource();
      _sbSource.buffer = previewBuf;
      _sbSource.loop = true;
      _sbSource.loopStart = 0;
      _sbSource.loopEnd = previewBuf.duration;
      const rpmSlider = document.getElementById('sbRpmSlider');
      _sbSource.playbackRate.value = rpmSlider ? parseFloat(rpmSlider.value) : 1.0;
      _sbConnectChain(_sbSource, ctx);
      _sbSource.onended = function() { _sbPlaying = false; updatePlayBtn(); };
      _sbSource.start(0);
      _sbLoopStartTime = ctx.currentTime;
      _sbPlaying = true;
      updatePlayBtn();
      return;
    }
  }

  _sbSource = ctx.createBufferSource();
  _sbSource.buffer = _sbBuffer;  // use the original full buffer
  _sbSource.loop = doLoop;
  _sbSource.loopStart = ls;
  _sbSource.loopEnd = le;
  const rpmSlider = document.getElementById('sbRpmSlider');
  _sbSource.playbackRate.value = rpmSlider ? parseFloat(rpmSlider.value) : 1.0;
  _sbConnectChain(_sbSource, ctx);
  _sbSource.onended = function() { _sbPlaying = false; updatePlayBtn(); };
  _sbSource.start(0, ls);  // start playback from loop start position
  _sbLoopStartTime = ctx.currentTime;
  _sbPlaying = true;
  updatePlayBtn();
}

// Resample PCM int8 array by a factor (>1 = more samples = lower pitch, <1 = fewer = higher)
function _sbResample(samples, factor) {
  if (Math.abs(factor - 1.0) < 0.001) return samples;
  const newLen = Math.max(4, Math.round(samples.length * factor));
  const out = new Int8Array(newLen);
  for (let i = 0; i < newLen; i++) {
    const srcPos = i / factor;
    const idx = Math.floor(srcPos);
    const frac = srcPos - idx;
    const s0 = idx < samples.length ? samples[idx] : 0;
    const s1 = (idx + 1) < samples.length ? samples[idx + 1] : s0;
    out[i] = Math.max(-128, Math.min(127, Math.round(s0 * (1 - frac) + s1 * frac)));
  }
  return out;
}

function sbUpdatePitch() {
  const pitchSlider = document.getElementById('sbPitch');
  const pitchLabel = document.getElementById('sbPitchLabel');
  const v = parseFloat(pitchSlider ? pitchSlider.value : '0');
  if (pitchLabel) pitchLabel.textContent = (v >= 0 ? '+' : '') + v.toFixed(1) + ' st';
  if (_sbPlaying) {
    if (_sbSwapTimer) clearTimeout(_sbSwapTimer);
    _sbSwapTimer = setTimeout(_sbHotSwap, 120);
  }
}

// ─── Engine Synthesizer ─────────────────────────────────────────
const _SYNTH_ENGINES = {
  single: { name:'Single Cylinder', cyl:1, order:[1], deg:720 },
  twin:   { name:'Parallel Twin',   cyl:2, order:[1,2], deg:360 },
  i3:     { name:'Inline 3',        cyl:3, order:[1,2,3], deg:240 },
  i4:     { name:'Inline 4',        cyl:4, order:[1,3,4,2], deg:180 },
  boxer4: { name:'Flat 4 (Boxer)',   cyl:4, order:[1,3,2,4], deg:180 },
  i6:     { name:'Inline 6',        cyl:6, order:[1,5,3,6,2,4], deg:120 },
  v6:     { name:'V6 60deg',        cyl:6, order:[1,4,2,5,3,6], deg:120 },
  v8xp:   { name:'V8 Cross-plane',  cyl:8, order:[1,8,4,3,6,5,7,2], deg:90 },
  v8fp:   { name:'V8 Flat-plane',   cyl:8, order:[1,5,4,8,3,7,2,6], deg:90 },
  v10:    { name:'V10',             cyl:10,order:[1,6,5,10,2,7,3,8,4,9], deg:72 },
  v12:    { name:'V12',             cyl:12,order:[1,7,5,11,3,9,6,12,2,8,4,10], deg:60 },
};

function synthUpdateLabel() {
  const type = document.getElementById('synthEngine').value;
  const cfg = _SYNTH_ENGINES[type];
  const el = document.getElementById('synthOrderLabel');
  if (el && cfg) el.textContent = 'Firing: ' + cfg.order.join('-') + ' (' + cfg.deg + '\u00b0 apart)';
}
// Init label on load
setTimeout(synthUpdateLabel, 500);

let _synthRegenTimer = null;
let _synthAutoEnabled = false;
function synthAutoRegen() {
  if (!_synthAutoEnabled) return;
  if (_synthRegenTimer) clearTimeout(_synthRegenTimer);
  _synthRegenTimer = setTimeout(synthGenerateEngine, 200);
}
// Event delegation: all synth sliders auto-regen after first Generate
setTimeout(function() {
  var sp = document.getElementById('synthPanel');
  if (sp) {
    sp.addEventListener('input', function(e) {
      var id = e.target.id;
      if (id==='synthRpm') document.getElementById('synthRpmVal').textContent=e.target.value;
      else if (id==='synthCombustion') document.getElementById('synthCombVal').textContent=e.target.value+'%';
      else if (id==='synthExhaustFreq') document.getElementById('synthExhVal').textContent=e.target.value+'Hz';
      else if (id==='synthInjector') document.getElementById('synthInjVal').textContent=e.target.value+'%';
      else if (id==='synthMechanical') document.getElementById('synthMechVal').textContent=e.target.value+'%';
      else if (id==='synthRoughness') document.getElementById('synthRoughVal').textContent=e.target.value+'%';
      else if (id==='synthResonance') document.getElementById('synthResVal').textContent=e.target.value;
      else if (id==='synthDrive') document.getElementById('synthDriveVal').textContent=e.target.value+'%';
      if (id && id.startsWith('synth')) synthAutoRegen();
    });
    sp.addEventListener('change', function(e) { if (e.target.tagName==='SELECT') { synthUpdateLabel(); synthAutoRegen(); } });
  }
}, 600);

function synthGenerateEngine() {
  const type = document.getElementById('synthEngine').value;
  const cfg = _SYNTH_ENGINES[type];
  if (!cfg) return;
  _synthAutoEnabled = true;
  const status = document.getElementById('synthStatus');

  const rpm = parseInt(document.getElementById('synthRpm').value) || 800;
  const SR = 22050;
  const strokeType = parseInt(document.getElementById('synthStroke').value) || 4;
  const numCycles = parseInt(document.getElementById('synthCycles').value) || 2;
  const cycleDeg = strokeType === 2 ? 360 : 720;
  const cycleSeconds = (60.0 / rpm) * (cycleDeg / 360);
  const cycleSamples = Math.round(cycleSeconds * SR);
  const totalSamples = cycleSamples * numCycles;
  if (totalSamples < 10 || totalSamples > 500000) {
    if (status) status.textContent = 'Bad RPM/cycle';
    return;
  }

  const combustion = (parseInt(document.getElementById('synthCombustion').value) || 75) / 100;
  const exhaustHz  = parseInt(document.getElementById('synthExhaustFreq').value) || 180;
  const injector   = (parseInt(document.getElementById('synthInjector').value) || 25) / 100;
  const mechanical = (parseInt(document.getElementById('synthMechanical').value) || 20) / 100;
  const roughness  = (parseInt(document.getElementById('synthRoughness').value) || 15) / 100;
  const resQ       = parseFloat(document.getElementById('synthResonance').value) || 5;
  const drive      = (parseInt(document.getElementById('synthDrive').value) || 40) / 100;

  const out = new Float32Array(totalSamples);

  // Per-cylinder variation seeds
  const seeds = [];
  for (let c = 0; c < cfg.cyl; c++) {
    seeds.push({
      vol: 1.0 - roughness * (Math.random() * 0.45),
      pitch: 1.0 + (Math.random() - 0.5) * roughness * 0.22,
      timing: Math.round((Math.random() - 0.5) * roughness * 25),
    });
  }

  // Biquad bandpass (Direct Form I) - simulates resonant exhaust pipe
  function bpf(input, freq, Q) {
    if (freq < 20 || freq >= SR * 0.45) return input;
    const w0 = 2 * Math.PI * freq / SR;
    const s = Math.sin(w0), co = Math.cos(w0);
    const al = s / (2 * Q);
    const b0 = al, b2 = -al, a0 = 1+al, a1 = -2*co, a2 = 1-al;
    let x1=0,x2=0,y1=0,y2=0;
    const r = new Float32Array(input.length);
    for (let i = 0; i < input.length; i++) {
      r[i] = (b0*input[i] + b2*x2 - a1*y1 - a2*y2) / a0;
      x2=x1; x1=input[i]; y2=y1; y1=r[i];
    }
    return r;
  }

  for (let cycle = 0; cycle < numCycles; cycle++) {
    const base = cycle * cycleSamples;
    for (let fireIdx = 0; fireIdx < cfg.order.length; fireIdx++) {
      const cylNum = cfg.order[fireIdx];
      const seed = seeds[cylNum - 1];
      const angle = fireIdx * cfg.deg;
      const nomOff = Math.round((angle / cycleDeg) * cycleSamples);
      const jitter = Math.round((Math.random() - 0.5) * roughness * 12);
      const fireOff = base + ((nomOff + seed.timing + jitter + cycleSamples) % cycleSamples);

      // === COMBUSTION: noise burst -> resonant bandpass (not sines!) ===
      const combMs = 6 + 20 * Math.max(0, 1 - rpm / 6000);
      const combLen = Math.min(Math.round(combMs * SR / 1000), Math.round(cycleSamples * 0.7));

      // Raw noise burst with asymmetric envelope
      const raw = new Float32Array(combLen);
      for (let i = 0; i < combLen; i++) {
        const tSec = i / SR;
        const t = i / combLen;
        const attack = Math.min(1.0, tSec / 0.0004); // 0.4ms attack
        const decay1 = Math.exp(-t * 5.5);            // fast primary
        const decay2 = Math.exp(-t * 1.5) * 0.35;     // slow rumble tail
        raw[i] = (Math.random() * 2 - 1) * attack * (decay1 + decay2);
      }

      // Filter through resonant bandpass (sounds like a real exhaust pipe!)
      const cylFreq = exhaustHz * seed.pitch;
      const filtered = bpf(raw, cylFreq, resQ);
      // Sub-bass: half frequency, lower Q for chest thump
      const subFiltered = bpf(raw, Math.max(30, cylFreq * 0.5), Math.max(1, resQ * 0.4));
      // Second resonance peak (pipe overtone)
      const overtone = bpf(raw, Math.min(cylFreq * 2.3, SR * 0.4), Math.max(1, resQ * 0.6));

      // Mix: primary resonance + sub + overtone + raw transient
      for (let i = 0; i < combLen; i++) {
        const idx = fireOff + i;
        if (idx >= totalSamples) break;
        const mixed = filtered[i]*0.55 + subFiltered[i]*0.22 + overtone[i]*0.12 + raw[i]*0.11;
        out[idx] += mixed * combustion * seed.vol * 3.0;
      }

      // === INJECTOR CLICK ===
      if (injector > 0.01) {
        const injLen = Math.round(0.8 * SR / 1000);
        const injOff = fireOff - Math.round(2.5 * SR / 1000);
        for (let i = 0; i < injLen; i++) {
          const idx = ((injOff+i) % totalSamples + totalSamples) % totalSamples;
          const t = i / injLen;
          const env = (1-t)*(1-t)*(1-t);
          const click = Math.sin(2*Math.PI*3200*i/SR)*0.5 + Math.sin(2*Math.PI*5800*i/SR)*0.3 + (Math.random()*2-1)*0.2;
          out[idx] += click * env * injector * 0.22 * seed.vol;
        }
      }

      // === VALVE TRAIN TICKS ===
      if (mechanical > 0.01) {
        for (let v = 0; v < 2; v++) {
          const mOff = fireOff + (v===0 ? 0 : Math.round(cycleSamples*0.5));
          const mLen = Math.round(1.5 * SR / 1000);
          for (let i = 0; i < mLen; i++) {
            const idx = ((mOff+i) % totalSamples + totalSamples) % totalSamples;
            const t = i / mLen;
            const env = Math.exp(-t * 12);
            const tick = (Math.random()*2-1)*0.6 + Math.sin(2*Math.PI*2200*i/SR)*0.4;
            out[idx] += tick * env * mechanical * 0.1 * seed.vol;
          }
        }
      }
    }
  }

  // === BACKGROUND MECHANICAL RUMBLE ===
  if (mechanical > 0.05) {
    const firingHz = (rpm/60) * (cfg.cyl / (strokeType===2 ? 1 : 2));
    for (let i = 0; i < totalSamples; i++) {
      const mod = 0.5 + 0.5*Math.sin(2*Math.PI*firingHz*i/SR);
      out[i] += (Math.random()*2-1) * mechanical * 0.035 * mod;
    }
  }

  // === POST-PROCESSING ===
  // 1) DC blocking highpass (~30Hz)
  let dcPrev = 0, dcOut = 0;
  for (let i = 0; i < totalSamples; i++) {
    dcOut = 0.995 * (dcOut + out[i] - dcPrev);
    dcPrev = out[i];
    out[i] = dcOut;
  }

  // 2) Soft saturation (tanh waveshaping for analog warmth)
  if (drive > 0.01) {
    const g = 1 + drive * 5;
    for (let i = 0; i < totalSamples; i++) out[i] = Math.tanh(out[i] * g);
  }

  // 3) Gentle lowpass to smooth aliasing
  const lpCut = Math.min(exhaustHz * 4, SR * 0.42);
  const lpRc = 1/(2*Math.PI*lpCut), lpDt = 1/SR, lpA = lpDt/(lpRc+lpDt);
  let lpP = 0;
  for (let i = 0; i < totalSamples; i++) {
    lpP += lpA * (out[i] - lpP);
    out[i] = out[i]*0.35 + lpP*0.65;
  }

  // 4) Normalize
  let mx = 0;
  for (let i = 0; i < totalSamples; i++) mx = Math.max(mx, Math.abs(out[i]));
  if (mx > 0.001) {
    const sc = 0.92 / mx;
    for (let i = 0; i < totalSamples; i++) out[i] *= sc;
  }

  // Convert to Int8
  const pcm8 = new Int8Array(totalSamples);
  for (let i = 0; i < totalSamples; i++) {
    pcm8[i] = Math.max(-128, Math.min(127, Math.round(out[i] * 127)));
  }

  // Load into editor
  _sbRawSamples = pcm8;
  _sbRawRate = SR;
  sbStop();
  const ctx = _getAudioCtx();
  _sbBuffer = ctx.createBuffer(1, totalSamples, SR);
  const ch = _sbBuffer.getChannelData(0);
  for (let i = 0; i < totalSamples; i++) ch[i] = out[i];

  _sbCurrentFile = 'synth_' + type + '_' + rpm + 'rpm';
  const nameEl = document.getElementById('sbNowPlaying');
  if (nameEl) nameEl.textContent = _sbCurrentFile;
  const infoEl = document.getElementById('sbSoundInfo');
  if (infoEl) infoEl.textContent = totalSamples + ' smp, ' + SR + 'Hz, ' + (totalSamples/SR).toFixed(2) + 's';

  const ls = document.getElementById('sbLoopStart');
  const le = document.getElementById('sbLoopEnd');
  if (ls) ls.value = 0;
  if (le) le.value = 1;
  sbUpdateLoopPoints();

  const loopCb = document.getElementById('sbLoop');
  if (loopCb) loopCb.checked = true;
  sbPlay();

  if (status) status.textContent = cfg.name + ' @ ' + rpm + ' RPM \u2014 ' + totalSamples + ' smp, ' + (totalSamples/SR).toFixed(2) + 's';
}

// Build a crossfaded Float32 AudioBuffer for preview from current slider positions
function _sbBuildPreviewBuf(ctx) {
  if (!_sbRawSamples) return null;
  const dur = _sbBuffer.duration;
  const startSlider = document.getElementById('sbLoopStart');
  const endSlider = document.getElementById('sbLoopEnd');
  const ls = parseFloat(startSlider ? startSlider.value : '0');
  const le = parseFloat(endSlider ? endSlider.value : '1');
  const startIdx = Math.max(0, Math.floor(ls * _sbRawSamples.length));
  const endIdx = Math.min(_sbRawSamples.length, Math.floor(le * _sbRawSamples.length));
  if (endIdx - startIdx < 4) return null;

  let slice = _sbRawSamples.slice(startIdx, endIdx);

  // Always apply at least a tiny crossfade for click-free looping
  const cfPct = parseInt(document.getElementById('sbCrossfade').value) || 0;
  const minFade = 32;  // minimum samples to smooth loop boundary
  if (slice.length > 200) {
    const fadeSamples = Math.max(minFade, Math.floor(slice.length * cfPct / 100));
    if (slice.length > fadeSamples * 2) {
      slice = crossfadeLoop(slice, fadeSamples);
    }
  }

  // Pitch shifting via resampling
  const pitchSt = parseFloat(document.getElementById('sbPitch') ? document.getElementById('sbPitch').value : '0');
  const pitchLock = !!(document.getElementById('sbPitchLock') && document.getElementById('sbPitchLock').checked);
  let pitchFactor = Math.pow(2, -pitchSt / 12);  // negative semitones = stretch = lower pitch
  if (pitchLock) {
    // Compensate for RPM so pitch stays constant: undo the playbackRate pitch shift
    const rpm = parseFloat(document.getElementById('sbRpmSlider') ? document.getElementById('sbRpmSlider').value : '1');
    pitchFactor *= (1 / rpm);
  }
  if (Math.abs(pitchFactor - 1.0) > 0.001) {
    slice = _sbResample(slice, pitchFactor);
  }

  const buf = ctx.createBuffer(1, slice.length, _sbRawRate);
  const ch = buf.getChannelData(0);
  for (let i = 0; i < slice.length; i++) ch[i] = slice[i] / 128.0;
  return buf;
}

function sbStop() {
  if (_sbSource) { try { _sbSource.stop(); } catch(e) {} _sbSource = null; }
  if (_sbHighPass) { try { _sbHighPass.disconnect(); } catch(e) {} _sbHighPass = null; }
  if (_sbLowPass) { try { _sbLowPass.disconnect(); } catch(e) {} _sbLowPass = null; }
  if (_sbGain) { try { _sbGain.disconnect(); } catch(e) {} _sbGain = null; }
  _sbPlaying = false;
  updatePlayBtn();
}

function sbPlayStop() {
  if (_sbPlaying) sbStop(); else sbPlay();
}

function updatePlayBtn() {
  const btn = document.getElementById('sbPlayBtn');
  if (btn) btn.innerHTML = _sbPlaying ? '&#9724; Stop' : '&#9654; Play';
}

function sbUpdateRpm(val) {
  const v = parseFloat(val);
  const label = document.getElementById('sbRpmLabel');
  let desc = 'idle';
  if (v >= 2.5) desc = 'redline';
  else if (v >= 2.0) desc = 'high RPM';
  else if (v >= 1.5) desc = 'mid-high';
  else if (v >= 1.1) desc = 'mid RPM';
  else if (v >= 0.7) desc = 'idle';
  else desc = 'very low';
  if (label) label.textContent = v.toFixed(2) + 'x (' + desc + ')';
  if (_sbSource && _sbPlaying) {
    const ctx = _getAudioCtx();
    _sbSource.playbackRate.setValueAtTime(_sbSource.playbackRate.value, ctx.currentTime);
    _sbSource.playbackRate.linearRampToValueAtTime(v, ctx.currentTime + 0.15);
    // When pitch lock is on, rebuild buffer to compensate for new RPM
    const pitchLock = !!(document.getElementById('sbPitchLock') && document.getElementById('sbPitchLock').checked);
    if (pitchLock) {
      if (_sbSwapTimer) clearTimeout(_sbSwapTimer);
      _sbSwapTimer = setTimeout(_sbHotSwap, 200);
    }
  }
}

function sbUpdateVol(val) {
  const v = parseInt(val);
  const label = document.getElementById('sbVolLabel');
  if (label) label.textContent = v + '%';
  if (_sbGain) {
    const ctx = _getAudioCtx();
    _sbGain.gain.setValueAtTime(_sbGain.gain.value, ctx.currentTime);
    _sbGain.gain.linearRampToValueAtTime(v / 100, ctx.currentTime + 0.08);
  }
}

let _sbRawSamples = null;  // original int8 samples from server
let _sbRawRate = 22050;

function sbUpdateLoopPoints() {
  if (!_sbBuffer) return;
  const dur = _sbBuffer.duration;
  const startSlider = document.getElementById('sbLoopStart');
  const endSlider = document.getElementById('sbLoopEnd');
  const startLabel = document.getElementById('sbLoopStartLabel');
  const endLabel = document.getElementById('sbLoopEndLabel');
  const selInfo = document.getElementById('sbSelectionInfo');

  let ls = parseFloat(startSlider.value) * dur;
  let le = parseFloat(endSlider.value) * dur;
  if (le <= ls) le = Math.min(ls + 0.001, dur);

  if (startLabel) startLabel.textContent = ls.toFixed(3) + 's';
  if (endLabel) endLabel.textContent = le.toFixed(3) + 's';

  const selSamples = Math.round((le - ls) * _sbRawRate);
  const selKB = Math.round(selSamples / 1024);
  if (selInfo) selInfo.textContent = 'Selection: ' + (le - ls).toFixed(3) + 's, ~' + selSamples + ' samples, ~' + selKB + ' KB';

  // Live update: debounce hot-swap with new buffer
  if (_sbPlaying && _sbSource) {
    if (_sbSwapTimer) clearTimeout(_sbSwapTimer);
    _sbSwapTimer = setTimeout(_sbHotSwap, 120);
  }
}

// Seamlessly swap the playing buffer with a freshly crossfaded one
function _sbHotSwap() {
  if (!_sbPlaying || !_sbSource || !_sbBuffer) return;
  const ctx = _getAudioCtx();
  const newBuf = _sbBuildPreviewBuf(ctx);
  if (!newBuf) return;

  // Figure out current phase so we restart at the same relative position
  const rate = _sbSource.playbackRate.value;
  const elapsed = (ctx.currentTime - _sbLoopStartTime) * rate;
  const oldDur = _sbLoopBuf ? _sbLoopBuf.duration : _sbBuffer.duration;
  const phase = elapsed % oldDur;
  const offset = Math.min(phase / oldDur * newBuf.duration, newBuf.duration - 0.001);

  const fadeTime = 0.03;
  const volSlider = document.getElementById('sbVolSlider');
  const vol = (volSlider ? parseInt(volSlider.value) : 100) / 100;

  // Kill old source's onended so it doesn't reset _sbPlaying
  const oldSource = _sbSource;
  const oldGain = _sbGain;
  oldSource.onended = null;
  oldGain.gain.setValueAtTime(oldGain.gain.value, ctx.currentTime);
  oldGain.gain.linearRampToValueAtTime(0, ctx.currentTime + fadeTime);
  try { oldSource.stop(ctx.currentTime + fadeTime + 0.01); } catch(e) {}
  // oldGain/oldHighPass/oldLowPass will be garbage collected once disconnected
  setTimeout(function(){ try { oldGain.disconnect(); } catch(e){} }, 100);

  // Start new source NOW (overlapping with old fade-out = no gap)
  _sbLoopBuf = newBuf;
  _sbSource = ctx.createBufferSource();
  _sbSource.buffer = newBuf;
  _sbSource.loop = true;
  _sbSource.loopStart = 0;
  _sbSource.loopEnd = newBuf.duration;
  _sbSource.playbackRate.value = rate;
  _sbConnectChain(_sbSource, ctx);
  _sbGain.gain.setValueAtTime(0, ctx.currentTime);
  _sbGain.gain.linearRampToValueAtTime(vol, ctx.currentTime + fadeTime);
  _sbSource.onended = function() { _sbPlaying = false; updatePlayBtn(); };
  _sbSource.start(0, offset > 0 ? offset : 0);
  _sbLoopStartTime = ctx.currentTime - (offset / (rate || 1));
}

function sbGetLoopRegion() {
  if (!_sbBuffer) return null;
  const dur = _sbBuffer.duration;
  const startSlider = document.getElementById('sbLoopStart');
  const endSlider = document.getElementById('sbLoopEnd');
  const ls = parseFloat(startSlider ? startSlider.value : '0') * dur;
  const le = parseFloat(endSlider ? endSlider.value : '1') * dur;
  return { start: ls, end: le };
}

function sbProcessSlice() {
  // Get loop region and slice raw samples
  const region = sbGetLoopRegion();
  const startIdx = Math.max(0, Math.floor(region.start * _sbRawRate));
  const endIdx = Math.min(_sbRawSamples.length, Math.floor(region.end * _sbRawRate));
  if (endIdx <= startIdx) return null;
  let slice = _sbRawSamples.slice(startIdx, endIdx);

  // Speed resample — uses the RPM slider value
  const speed = parseFloat(document.getElementById('sbRpmSlider').value) || 1;
  let outRate = parseInt(document.getElementById('sbExportRate').value) || 22050;
  if (speed !== 1) {
    // Convert to float, resample, convert back
    const floats = new Float32Array(slice.length);
    for (let i = 0; i < slice.length; i++) floats[i] = slice[i] / 128.0;
    const resampled = resampleLinear(floats, _sbRawRate * speed, _sbRawRate);
    slice = new Int8Array(resampled.length);
    for (let i = 0; i < resampled.length; i++) slice[i] = Math.max(-128, Math.min(127, Math.round(resampled[i] * 128)));
  }

  // Rate conversion (if output rate differs from source)
  if (outRate !== _sbRawRate) {
    const floats = new Float32Array(slice.length);
    for (let i = 0; i < slice.length; i++) floats[i] = slice[i] / 128.0;
    const resampled = resampleLinear(floats, _sbRawRate, outRate);
    slice = new Int8Array(resampled.length);
    for (let i = 0; i < resampled.length; i++) slice[i] = Math.max(-128, Math.min(127, Math.round(resampled[i] * 128)));
  }

  // Smooth (compress dynamic range) — before normalize
  const smoothPct = parseInt(document.getElementById('sbSmooth').value) || 0;
  if (smoothPct > 0) {
    slice = compressPcm8(slice, smoothPct / 100);
  }

  // Normalize
  if (document.getElementById('sbExportNorm').checked) {
    let peak = 0;
    for (let i = 0; i < slice.length; i++) peak = Math.max(peak, Math.abs(slice[i]));
    if (peak > 0 && peak < 127) {
      const gain = 127 / peak;
      const normed = new Int8Array(slice.length);
      for (let i = 0; i < slice.length; i++) normed[i] = Math.max(-128, Math.min(127, Math.round(slice[i] * gain)));
      slice = normed;
    }
  }

  // Crossfade LAST: blend end→start for seamless loop (must be after resample + normalize)
  const cfPct = parseInt(document.getElementById('sbCrossfade').value) || 0;
  if (cfPct > 0 && slice.length > 200) {
    const fadeSamples = Math.max(2, Math.floor(slice.length * cfPct / 100));
    slice = crossfadeLoop(slice, fadeSamples);
  }

  return { slice: slice, rate: outRate, region: region, speed: speed };
}

function sbBuildHeader(varName, slice, rate, region, speed) {
  const lines = [];
  lines.push('// Exported from Sound Browser — ' + _sbCurrentFile);
  lines.push('// Loop region: ' + region.start.toFixed(3) + 's - ' + region.end.toFixed(3) + 's');
  if (speed !== 1) lines.push('// Export speed: ' + speed + 'x');
  lines.push('#pragma once');
  lines.push('const unsigned int ' + varName + '_sampleRate = ' + rate + ';');
  lines.push('const unsigned int ' + varName + '_sampleCount = ' + slice.length + ';');
  lines.push('const signed char ' + varName + '[] = {');
  let row = '  ';
  for (let i = 0; i < slice.length; i++) {
    row += slice[i].toString();
    if (i !== slice.length - 1) row += ', ';
    if ((i + 1) % 20 === 0 && i !== slice.length - 1) { lines.push(row); row = '  '; }
  }
  if (row.trim()) lines.push(row);
  lines.push('};');
  return lines.join('\n');
}

function sbExportSelection() {
  if (!_sbRawSamples || !_sbBuffer) { status('Load a sound first', 'warn'); return; }
  const p = sbProcessSlice();
  if (!p) { status('Invalid selection', 'warn'); return; }
  let veh = getSelectedVehicleFile();
  let prefix = veh ? veh.replace(/\.h$/i, '') : 'custom';
  const varName = prefix + _sbCurrentFile.replace('.h', '').replace(/[^a-zA-Z0-9]/g, '');
  const text = sbBuildHeader(varName, p.slice, p.rate, p.region, p.speed);
  const blob = new Blob([text], {type: 'text/plain'});
  const a = document.createElement('a');
  a.href = URL.createObjectURL(blob);
  a.download = varName + '.h';
  a.click();
  URL.revokeObjectURL(a.href);
  const sizeKB = Math.round(p.slice.length / 1024);
  const dur = (p.slice.length / p.rate).toFixed(2);
  status('Exported ' + varName + '.h — ' + p.slice.length + ' samples (' + dur + 's @ ' + p.rate + 'Hz), ~' + sizeKB + ' KB', sizeKB > 500 ? 'warn' : '');
}

async function refreshVehicleSection() {
  const veh = getSelectedVehicleFile();
  if (!veh) return;
  const oldDetails = document.getElementById('selectedVehicleDetails');
  if (!oldDetails) return;
  // Snapshot open <details> sections and scroll position
  const scrollY = window.scrollY || window.pageYOffset;
  const openSet = new Set();
  oldDetails.querySelectorAll('details[open]').forEach(d => {
    const s = d.querySelector('summary');
    if (s) openSet.add(s.textContent.trim());
  });
  const wasOpen = oldDetails.hasAttribute('open');
  try {
    const sec = await fetch('/vehicle_section?vehicle=' + encodeURIComponent(veh));
    const j = await sec.json();
    if (!j.ok) return;
    const tmp = document.createElement('div');
    tmp.innerHTML = j.html;
    const newDetails = tmp.firstElementChild;
    if (!newDetails) return;
    oldDetails.replaceWith(newDetails);
    // Restore open state on the outer <details>
    if (wasOpen) newDetails.setAttribute('open', '');
    // Restore inner <details> open states
    newDetails.querySelectorAll('details').forEach(d => {
      const s = d.querySelector('summary');
      if (s && openSet.has(s.textContent.trim())) d.setAttribute('open', '');
    });
    setAdvancedSoundVisibility(showAdvancedSound);
    updateVehicleNameDisplay();
    // Restore scroll position
    window.scrollTo(0, scrollY);
  } catch(e) { /* silent — install already succeeded */ }
}

async function sbInstallSelection() {
  if (!_sbRawSamples || !_sbBuffer) { status('Load a sound first', 'warn'); return; }
  const catSel = document.getElementById('sbInstallCategory');
  const catKey = catSel ? catSel.value : '';
  if (!catKey) { status('Pick a category from "Install as" first', 'warn'); return; }
  const p = sbProcessSlice();
  if (!p) { status('Invalid selection', 'warn'); return; }

  // Use the category-specific filename from the dropdown data attribute
  const catOpt = catSel.options[catSel.selectedIndex];
  const catFilename = (catOpt && catOpt.dataset.filename) ? catOpt.dataset.filename : '';
  let filename;
  if (catFilename) {
    filename = catFilename + '.h';
  } else {
    // Fallback: vehicle name + source name
    let veh = getSelectedVehicleFile();
    let prefix = veh ? veh.replace(/\.h$/i, '') : 'custom';
    let baseName = _sbCurrentFile.replace('.h', '').replace(/[^a-zA-Z0-9_]/g, '');
    filename = prefix + baseName + '.h';
  }
  const varName = filename.replace('.h', '');
  const text = sbBuildHeader(varName, p.slice, p.rate, p.region, p.speed);

  try {
    const r = await fetch('/install_header', {
      method: 'POST',
      headers: {'Content-Type': 'application/json'},
      body: JSON.stringify({filename: filename, text: text, category: catKey})
    });
    const j = await r.json();
    if (j.ok) {
      const sizeKB = Math.round(p.slice.length / 1024);
      const dur = (p.slice.length / p.rate).toFixed(2);
      status('Installed ' + (j.filename || filename) + ' as ' + catKey + ' — ' + p.slice.length + ' samples (' + dur + 's @ ' + p.rate + 'Hz), ~' + sizeKB + ' KB', '');
      // Refresh vehicle section so the new sound shows up
      await refreshVehicleSection();
    } else {
      status('Install failed: ' + (j.error || '?'), 'bad');
    }
  } catch(e) { status('Install error: ' + e, 'bad'); }
}

loadPorts();
loadVolume();
updateVehicleNameDisplay();
setAdvancedSoundVisibility(false);
switchMainTab('config');
autoLoadLastSession();
</script>
</body>
</html>
"""


def build_sections_html():
    vehicles = get_vehicle_list()
    current_vehicle = get_current_vehicle()
    parts = []

    for rel in CONFIG_FILES:
        label = FILE_LABELS.get(rel, rel)
        section_html = render_section_html(rel, label, vehicles, current_vehicle, selected_vehicle=False)
        if section_html:
            parts.append(section_html)

    if current_vehicle:
        rel = "vehicles/" + current_vehicle
        label = "Selected Vehicle: " + current_vehicle
        section_html = render_section_html(rel, label, vehicles, current_vehicle, selected_vehicle=True)
        if section_html:
            parts.append(section_html)

    return "\n".join(parts)


def build_selected_vehicle_section_html(vehicle_file):
    vehicles = get_vehicle_list()
    current_vehicle = vehicle_file
    rel = "vehicles/" + vehicle_file
    label = "Selected Vehicle: " + vehicle_file
    return render_section_html(rel, label, vehicles, current_vehicle, selected_vehicle=True)


def build_page():
    return PAGE_TEMPLATE.replace("__SECTIONS__", build_sections_html())


class Handler(BaseHTTPRequestHandler):
    def log_message(self, fmt, *args):
        pass

    def send_json(self, data, code=200):
        body = json.dumps(data).encode("utf-8")
        self.send_response(code)
        self.send_header("Content-Type", "application/json")
        self.send_header("Content-Length", str(len(body)))
        self.end_headers()
        self.wfile.write(body)

    def do_GET(self):
        if self.path.startswith("/download_vehicle"):
            try:
                parsed = urllib.parse.urlparse(self.path)
                q = urllib.parse.parse_qs(parsed.query)
                vehicle = os.path.basename((q.get("vehicle", [""])[0] or "").strip())
                if not vehicle or not vehicle.endswith(".h"):
                    self.send_json({"ok": False, "error": "Invalid vehicle file"}, 400)
                    return
                full = os.path.join(SRC, "vehicles", vehicle)
                if not os.path.exists(full):
                    self.send_json({"ok": False, "error": "Vehicle file not found"}, 404)
                    return
                body = read_text(full).encode("utf-8")
                self.send_response(200)
                self.send_header("Content-Type", "application/octet-stream")
                self.send_header("Content-Disposition", 'attachment; filename="%s"' % vehicle)
                self.send_header("Content-Length", str(len(body)))
                self.end_headers()
                self.wfile.write(body)
            except Exception as e:
                self.send_json({"ok": False, "error": str(e)}, 500)
            return

        if self.path.startswith("/vehicle_section"):
            try:
                parsed = urllib.parse.urlparse(self.path)
                q = urllib.parse.parse_qs(parsed.query)
                vehicle = os.path.basename((q.get("vehicle", [""])[0] or "").strip())

                if not vehicle or not vehicle.endswith(".h"):
                    self.send_json({"ok": False, "error": "Invalid vehicle file"}, 400)
                    return

                full = os.path.join(SRC, "vehicles", vehicle)
                if not os.path.exists(full):
                    self.send_json({"ok": False, "error": "Vehicle file not found"}, 404)
                    return

                html = build_selected_vehicle_section_html(vehicle)
                self.send_json({"ok": True, "html": html, "vehicle": vehicle})
            except Exception as e:
                self.send_json({"ok": False, "error": str(e)}, 500)
            return

        if self.path in ("/", "/index.html"):
            try:
                body = build_page().encode("utf-8")
            except Exception as e:
                body = ("Error building page: " + str(e)).encode("utf-8")
            self.send_response(200)
            self.send_header("Content-Type", "text/html; charset=utf-8")
            self.send_header("Content-Length", str(len(body)))
            self.end_headers()
            self.wfile.write(body)
            return

        if self.path.startswith("/tools/"):
            rel = self.path[len("/tools/"):].split("?", 1)[0].replace("\\", "/")
            rel = rel.lstrip("/")
            if not rel or ".." in rel:
                self.send_response(400)
                self.end_headers()
                return
            full = os.path.join(TOOLS, rel)
            if not os.path.exists(full) or not os.path.isfile(full):
                self.send_response(404)
                self.end_headers()
                return
            mime = "text/html; charset=utf-8" if full.lower().endswith(".html") else "application/octet-stream"
            if full.lower().endswith(".html"):
                data = read_text(full).encode("utf-8")
            else:
                with open(full, "rb") as fh:
                    data = fh.read()
            self.send_response(200)
            self.send_header("Content-Type", mime)
            self.send_header("Content-Length", str(len(data)))
            self.end_headers()
            self.wfile.write(data)
            return

        if self.path == "/ports":
            self.send_json({"ports": list_serial_ports(), "connected": CONNECTED_PORT})
            return

        if self.path == "/get_volume":
            try:
                sound_path = os.path.join(SRC, "8_Sound.h")
                text = read_text(sound_path)
                # Match only uncommented masterVolumePercentage line
                m = re.search(r"^[^/\n]*masterVolumePercentage\[\]\s*=\s*\{(\d+)", text, re.MULTILINE)
                vol = int(m.group(1)) if m else 100
                # Check if VOL_POT_OVERRIDE is uncommented (active)
                pot_override = bool(re.search(r"^\s*#define\s+VOL_POT_OVERRIDE", text, re.MULTILINE))
                self.send_json({"ok": True, "volume": vol, "potOverride": pot_override})
            except Exception as e:
                self.send_json({"ok": False, "error": str(e)})
            return

        if self.path == "/custom_sounds":
            try:
                veh_text = read_text(os.path.join(SRC, "1_Vehicle.h"))
                active_vehicle = None
                for vline in veh_text.splitlines():
                    vm = re.match(r'^\s*#include\s+"vehicles/([^"]+)"', vline)
                    if vm:
                        active_vehicle = vm.group(1)
                        break
                sounds = []
                if active_vehicle:
                    vf_path = os.path.join(SRC, "vehicles", active_vehicle)
                    if os.path.isfile(vf_path):
                        for ln in read_text(vf_path).splitlines():
                            m = re.match(r'^\s*(//\s*)?#include\s+"sounds/([^"]+)"\s*//\s*(.*)\(custom\)', ln)
                            if m:
                                sounds.append({
                                    "file": m.group(2),
                                    "category": m.group(3).strip(),
                                    "active": not bool(m.group(1)),
                                })
                self.send_json({"ok": True, "sounds": sounds})
            except Exception as e:
                self.send_json({"ok": False, "error": str(e), "sounds": []})
            return

        if self.path == "/all_sounds":
            try:
                all_s = scan_all_sounds()
                self.send_json({"ok": True, "sounds": all_s})
            except Exception as e:
                self.send_json({"ok": False, "error": str(e), "sounds": []})
            return

        if self.path.startswith("/sound_pcm/"):
            fn = os.path.basename(urllib.parse.unquote(self.path[len("/sound_pcm/"):]))
            if not fn.endswith(".h"):
                self.send_json({"ok": False, "error": "Not a .h file"}, 400)
                return
            fpath = os.path.join(SRC, "vehicles", "sounds", fn)
            if not os.path.isfile(fpath):
                self.send_json({"ok": False, "error": "File not found"}, 404)
                return
            try:
                data = parse_sound_header(fpath)
                if not data:
                    self.send_json({"ok": False, "error": "Could not parse sound header"})
                    return
                self.send_json({"ok": True, "file": fn, "sampleRate": data["sampleRate"],
                                "sampleCount": data["sampleCount"], "samples": data["samples"]})
            except Exception as e:
                self.send_json({"ok": False, "error": str(e)})
            return

        if self.path == "/sound_categories":
            # Return available sound categories from the active vehicle file
            # Filenames use vehicle name + sequential number, e.g. CaterpillarD6Dozer1
            CATEGORY_SUFFIX = {
                "start_sound": "Start",
                "idle_sound": "Idle",
                "motor_idle": "Idle",
                "revving_sound": "Rev",
                "motor_rev": "Rev",
                "jake_brake": "JakeBrake",
                "knock_sound": "Knock",
                "ignition_knock": "Knock",
                "siren": "Siren",
                "horn_sound": "Horn",
                "air_brake": "AirBrake",
                "parking_brake": "ParkingBrake",
                "gear_shifting": "Shifting",
                "sound1": "Sound1",
                "additional_sound": "Sound1",
                "reversing": "Reversing",
                "indicator": "Indicator",
                "turn_signal": "Indicator",
                "coupling": "Coupling",
                "hydraulic_pump": "HydPump",
                "hydraulic_fluid": "HydFlow",
                "hydraulic_flow": "HydFlow",
                "squeaky_track": "TrackRattle",
                "track_rattle_2": "TrackRattle2",
                "track_rattle": "TrackRattle",
                "bucket_rattle": "BucketRattle",
                "turbo": "Turbo",
                "wastegate": "Wastegate",
                "blowoff": "Wastegate",
                "supercharger": "Charger",
                "charger": "Charger",
                "fan_sound": "Fan",
                "cooling_fan": "Fan",
            }
            try:
                veh_text = read_text(os.path.join(SRC, "1_Vehicle.h"))
                active_vehicle = None
                for vline in veh_text.splitlines():
                    vm = re.match(r'^\s*#include\s+"vehicles/([^"]+)"', vline)
                    if vm:
                        active_vehicle = vm.group(1)
                        break
                cats = []
                if active_vehicle:
                    veh_base = re.sub(r'\.h$', '', active_vehicle)
                    sc = parse_sound_choices("vehicles/" + active_vehicle)
                    # Count existing files with this vehicle prefix to auto-number
                    sounds_dir = os.path.join(SRC, "vehicles", "sounds")
                    existing = set(f for f in os.listdir(sounds_dir) if f.startswith(veh_base)) if os.path.isdir(sounds_dir) else set()
                    for g in sc:
                        suffix = "Sound"
                        for pat, sfx in CATEGORY_SUFFIX.items():
                            if pat in g["key"]:
                                suffix = sfx
                                break
                        fname = veh_base + suffix
                        # If this filename already exists, append a number
                        if (fname + '.h') in existing:
                            n = 2
                            while (fname + str(n) + '.h') in existing:
                                n += 1
                            fname = fname + str(n)
                        # Clean up the display title
                        nice_title = re.sub(r"\s*\(uncomment.*", "", g["title"], flags=re.IGNORECASE).strip()
                        nice_title = re.sub(r"\s*\(played in.*", "", nice_title, flags=re.IGNORECASE).strip()
                        nice_title = re.sub(r"\s*uncomment\s+.*", "", nice_title, flags=re.IGNORECASE).strip()
                        nice_title = re.sub(r"\s*comment\s+.*out.*", "", nice_title, flags=re.IGNORECASE).strip()
                        nice_title = re.sub(r"\s*don.t\s+uncomment.*", "", nice_title, flags=re.IGNORECASE).strip()
                        nice_title = nice_title.strip(" -,")
                        nice_title = re.sub(r"^the\s+", "", nice_title, flags=re.IGNORECASE)
                        nice_title = nice_title[0].upper() + nice_title[1:] if nice_title else g["title"]
                        cats.append({"key": g["key"], "title": nice_title, "filename": fname})
                self.send_json({"ok": True, "categories": cats})
            except Exception as e:
                self.send_json({"ok": False, "error": str(e), "categories": []})
            return

        if self.path.startswith("/presets"):
          try:
            parsed = urllib.parse.urlparse(self.path)
            q = urllib.parse.parse_qs(parsed.query)
            vehicle = os.path.basename((q.get("vehicle", [""])[0] or "").strip())
            if not vehicle or not vehicle.endswith(".h"):
              self.send_json({"ok": False, "error": "Invalid vehicle file"}, 400)
              return
            self.send_json({"ok": True, "vehicle": vehicle, "presets": list_vehicle_presets(vehicle)})
          except Exception as e:
            self.send_json({"ok": False, "error": str(e)}, 500)
          return

        self.send_response(404)
        self.end_headers()

    def do_POST(self):
        global CONNECTED_PORT

        length = int(self.headers.get("Content-Length", "0"))
        body = self.rfile.read(length)

        if self.path == "/connect":
            try:
                payload = json.loads(body)
                port = str(payload.get("port", "")).strip()
                if not port:
                    self.send_json({"ok": False, "error": "No port selected"}, 400)
                    return
                # Verify port is accessible before storing it
                try:
                    import serial  # type: ignore
                    with serial.Serial(port=port, baudrate=115200, timeout=0.5):
                        pass
                except Exception as serial_err:
                    self.send_json({"ok": False, "error": "Cannot open %s: %s" % (port, serial_err)}, 400)
                    return
                CONNECTED_PORT = port
                self.send_json({"ok": True, "port": CONNECTED_PORT})
            except Exception as e:
                self.send_json({"ok": False, "error": str(e)}, 500)
            return

        if self.path == "/set_vehicle":
            try:
                payload = json.loads(body)
                vehicle = str(payload.get("vehicle", "")).strip()
                vehicle = os.path.basename(vehicle)
                if not vehicle or not vehicle.endswith(".h"):
                    self.send_json({"ok": False, "error": "Invalid vehicle file"}, 400)
                    return

                full = os.path.join(SRC, "vehicles", vehicle)
                if not os.path.exists(full):
                    self.send_json({"ok": False, "error": "Vehicle file not found"}, 404)
                    return

                apply_vehicle(vehicle)
                self.send_json({"ok": True, "vehicle": vehicle})
            except Exception as e:
                self.send_json({"ok": False, "error": str(e)}, 500)
            return


        if self.path == "/preset_save":
            try:
                payload = json.loads(body)
                vehicle = os.path.basename(str(payload.get("vehicle", "")).strip())
                name = str(payload.get("name", "")).strip()
                data = payload.get("data", {})
                if not vehicle or not vehicle.endswith(".h"):
                    self.send_json({"ok": False, "error": "Invalid vehicle file"}, 400)
                    return
                if not name:
                    self.send_json({"ok": False, "error": "Preset name is required"}, 400)
                    return
                if not isinstance(data, dict):
                    self.send_json({"ok": False, "error": "Preset payload must be an object"}, 400)
                    return

                path = preset_file_path(vehicle, name)
                write_text(
                    path,
                    json.dumps(
                        {
                            "vehicle": vehicle,
                            "name": name,
                            "savedAt": int(time.time()),
                            "data": data,
                        },
                        indent=2,
                    )
                    + "\n",
                )
                self.send_json({"ok": True, "name": safe_preset_token(name), "vehicle": vehicle})
            except Exception as e:
                self.send_json({"ok": False, "error": str(e)}, 500)
            return

        if self.path == "/preset_load":
            try:
                payload = json.loads(body)
                vehicle = os.path.basename(str(payload.get("vehicle", "")).strip())
                name = str(payload.get("name", "")).strip()
                if not vehicle or not vehicle.endswith(".h"):
                    self.send_json({"ok": False, "error": "Invalid vehicle file"}, 400)
                    return
                if not name:
                    self.send_json({"ok": False, "error": "Preset name is required"}, 400)
                    return

                path = preset_file_path(vehicle, name)
                if not os.path.exists(path):
                    self.send_json({"ok": False, "error": "Preset file not found"}, 404)
                    return
                raw = json.loads(read_text(path))
                data = raw.get("data", {})
                if not isinstance(data, dict):
                    self.send_json({"ok": False, "error": "Preset file format invalid"}, 400)
                    return
                self.send_json({"ok": True, "name": safe_preset_token(name), "vehicle": vehicle, "data": data})
            except Exception as e:
                self.send_json({"ok": False, "error": str(e)}, 500)
            return

        if self.path == "/preset_delete":
            try:
                payload = json.loads(body)
                vehicle = os.path.basename(str(payload.get("vehicle", "")).strip())
                name = str(payload.get("name", "")).strip()
                if not vehicle or not vehicle.endswith(".h"):
                    self.send_json({"ok": False, "error": "Invalid vehicle file"}, 400)
                    return
                if not name:
                    self.send_json({"ok": False, "error": "Preset name is required"}, 400)
                    return

                path = preset_file_path(vehicle, name)
                if not os.path.exists(path):
                    self.send_json({"ok": False, "error": "Preset file not found"}, 404)
                    return
                os.remove(path)
                self.send_json({"ok": True, "name": safe_preset_token(name), "vehicle": vehicle})
            except Exception as e:
                self.send_json({"ok": False, "error": str(e)}, 500)
            return

        if self.path == "/reset_vehicle":
            try:
                payload = json.loads(body)
                vehicle = os.path.basename(str(payload.get("vehicle", "")).strip())
                if not vehicle or not vehicle.endswith(".h"):
                    self.send_json({"ok": False, "error": "Invalid vehicle file"}, 400)
                    return
                bp = backup_path_for(vehicle)
                dest = os.path.join(SRC, "vehicles", vehicle)
                if not os.path.isfile(bp):
                    self.send_json({"ok": False, "error": "No backup found for " + vehicle}, 404)
                    return
                import shutil
                shutil.copy2(bp, dest)
                apply_vehicle(vehicle)
                self.send_json({"ok": True, "vehicle": vehicle})
            except Exception as e:
                self.send_json({"ok": False, "error": str(e)}, 500)
            return

        if self.path == "/export_vehicle":
            try:
                payload = json.loads(body)
                vehicle = os.path.basename(str(payload.get("vehicle", "")).strip())
                new_name = str(payload.get("newName", "")).strip()
                if not vehicle or not vehicle.endswith(".h"):
                    self.send_json({"ok": False, "error": "Invalid vehicle file"}, 400)
                    return
                src_path = os.path.join(SRC, "vehicles", vehicle)
                if not os.path.isfile(src_path):
                    self.send_json({"ok": False, "error": "Vehicle file not found"}, 404)
                    return
                if new_name:
                    # Sanitize filename
                    safe = re.sub(r"[^A-Za-z0-9_-]", "", new_name)
                    if not safe:
                        self.send_json({"ok": False, "error": "Invalid name"}, 400)
                        return
                    new_file = safe + ".h"
                    dest = os.path.join(SRC, "vehicles", new_file)
                    if os.path.exists(dest):
                        self.send_json({"ok": False, "error": new_file + " already exists"}, 400)
                        return
                    import shutil
                    shutil.copy2(src_path, dest)
                    self.send_json({"ok": True, "vehicle": vehicle, "newFile": new_file})
                else:
                    content = read_text(src_path)
                    self.send_json({"ok": True, "vehicle": vehicle, "content": content})
            except Exception as e:
                self.send_json({"ok": False, "error": str(e)}, 500)
            return

        if self.path == "/set_volume":
            try:
                req = json.loads(body)
                vol = max(0, min(250, int(req.get("volume", 100))))
                sound_path = os.path.join(SRC, "8_Sound.h")
                lines = read_text(sound_path).splitlines()
                # Build proportional volume steps: full, 75%, 50%, 0 (mute)
                steps = [vol, max(0, int(vol * 0.75)), max(0, int(vol * 0.50)), 0]
                new_arr = "{%s}" % ", ".join(str(s) for s in steps)
                for i, ln in enumerate(lines):
                    # Only replace uncommented masterVolumePercentage
                    if ln.lstrip().startswith("//"):
                        continue
                    if "masterVolumePercentage[]" in ln:
                        lines[i] = re.sub(
                            r"(masterVolumePercentage\[\]\s*=\s*)\{[^}]+\}",
                            r"\g<1>" + new_arr,
                            ln,
                        )
                        break
                write_text(sound_path, "\n".join(lines) + "\n")
                self.send_json({"ok": True, "volume": vol, "steps": steps})
            except Exception as e:
                self.send_json({"ok": False, "error": str(e)}, 500)
            return

        if self.path == "/set_vol_pot_override":
            try:
                req = json.loads(body)
                enabled = bool(req.get("enabled", False))
                sound_path = os.path.join(SRC, "8_Sound.h")
                lines = read_text(sound_path).splitlines()
                found = False
                for i, ln in enumerate(lines):
                    if "VOL_POT_OVERRIDE" in ln and "#define" in ln:
                        if enabled:
                            lines[i] = "#define VOL_POT_OVERRIDE"
                        else:
                            lines[i] = "// #define VOL_POT_OVERRIDE"
                        found = True
                        break
                if not found and enabled:
                    # Insert before the Volume adjustment comment
                    for i, ln in enumerate(lines):
                        if "Volume adjustment" in ln:
                            lines.insert(i, "#define VOL_POT_OVERRIDE")
                            found = True
                            break
                write_text(sound_path, "\n".join(lines) + "\n")
                self.send_json({"ok": True, "enabled": enabled})
            except Exception as e:
                self.send_json({"ok": False, "error": str(e)}, 500)
            return

        if self.path == "/delete_sound":
            try:
                req = json.loads(body)
                filename = os.path.basename(req.get("filename", "").strip())
                if not filename or not filename.endswith(".h"):
                    self.send_json({"ok": False, "error": "Invalid filename"}, 400)
                    return

                # Delete the file from sounds/
                fpath = os.path.join(SRC, "vehicles", "sounds", filename)
                if os.path.isfile(fpath):
                    os.remove(fpath)

                # Remove include line from active vehicle file
                removed = False
                veh_text = read_text(os.path.join(SRC, "1_Vehicle.h"))
                active_vehicle = None
                for vline in veh_text.splitlines():
                    vm = re.match(r'^\s*#include\s+"vehicles/([^"]+)"', vline)
                    if vm:
                        active_vehicle = vm.group(1)
                        break
                if active_vehicle:
                    vf_path = os.path.join(SRC, "vehicles", active_vehicle)
                    if os.path.isfile(vf_path):
                        vf_lines = read_text(vf_path).splitlines()
                        new_lines = []
                        for ln in vf_lines:
                            if re.match(r'^\s*(//\s*)?#include\s+"sounds/' + re.escape(filename) + r'"', ln):
                                removed = True
                                continue  # skip this line
                            new_lines.append(ln)
                        if removed:
                            write_text(vf_path, "\n".join(new_lines) + "\n")

                self.send_json({"ok": True, "removed_from_vehicle": removed})
            except Exception as e:
                self.send_json({"ok": False, "error": str(e)}, 500)
            return

        if self.path == "/install_header":
            try:
                req = json.loads(body)
                raw_name = req.get("filename", "").strip()
                text = req.get("text", "")
                category_key = req.get("category", "").strip()  # e.g. "the_track_rattle_2_sound"
                if not raw_name or not text:
                    self.send_json({"ok": False, "error": "filename and text required"}, 400)
                    return

                # Find the correct variable prefix for this category
                var_prefix = get_var_prefix_for_key(category_key) if category_key else None

                # Auto-rename variables in the text before saving
                if var_prefix is not None:
                    m_arr = re.search(r"const\s+signed\s+char\s+(\w+)\s*\[\]", text)
                    if m_arr:
                        old_name = m_arr.group(1)
                        if var_prefix == "":
                            new_arr, new_rate, new_count = "samples", "sampleRate", "sampleCount"
                        else:
                            new_arr = var_prefix + "Samples"
                            new_rate = var_prefix + "SampleRate"
                            new_count = var_prefix + "SampleCount"
                        text = text.replace(old_name + "[]", new_arr + "[]")
                        text = text.replace(old_name + " []", new_arr + "[]")
                        text = re.sub(r"\b" + re.escape(old_name) + r"_sampleRate\b", new_rate, text)
                        text = re.sub(r"\b" + re.escape(old_name) + r"SampleRate\b", new_rate, text)
                        text = re.sub(r"\b" + re.escape(old_name) + r"_sampleCount\b", new_count, text)
                        text = re.sub(r"\b" + re.escape(old_name) + r"SampleCount\b", new_count, text)
                        if new_count not in text:
                            m_body = re.search(r"const\s+signed\s+char\s+\w+\[\]\s*=\s*\{([^}]+)\}", text, re.DOTALL)
                            if m_body:
                                count = len([x for x in m_body.group(1).split(",") if x.strip()])
                                count_line = "const unsigned int %s = %d;\n" % (new_count, count)
                                # Insert before the array line
                                text = re.sub(
                                    r"(const\s+signed\s+char\s+)",
                                    count_line + r"\1",
                                    text,
                                    count=1,
                                )

                # sanitise: allow only alphanum, underscore, dot
                import re as _re
                safe_name = _re.sub(r"[^\w.]", "_", raw_name)
                if not safe_name.endswith(".h"):
                    safe_name += ".h"
                # Save to vehicles/sounds/ directory
                sounds_dir = os.path.join(SRC, "vehicles", "sounds")
                os.makedirs(sounds_dir, exist_ok=True)
                dest = os.path.join(sounds_dir, safe_name)
                with open(dest, "w", encoding="utf-8") as fh:
                    fh.write(text)

                # If a category was chosen, use apply_sound_choices to set it as active
                added_to_vehicle = False
                if category_key:
                    veh_text = read_text(os.path.join(SRC, "1_Vehicle.h"))
                    active_vehicle = None
                    for vline in veh_text.splitlines():
                        vm = re.match(r'^\s*#include\s+"vehicles/([^"]+)"', vline)
                        if vm:
                            active_vehicle = vm.group(1)
                            break
                    if active_vehicle:
                        vf_path = os.path.join(SRC, "vehicles", active_vehicle)
                        if os.path.isfile(vf_path):
                            # First, ensure the file exists as an option in the section
                            vf_lines = read_text(vf_path).splitlines()
                            found_in_section = False
                            in_section = False
                            insert_idx = None
                            for li, ln in enumerate(vf_lines):
                                mt = re.match(r"^\s*//\s*Choose\s+(.+)$", ln, flags=re.IGNORECASE)
                                if mt:
                                    sk = section_key(clean_comment(mt.group(1)))
                                    in_section = (sk == category_key)
                                    continue
                                if in_section and re.match(r'^\s*(//\s*)?#include\s+"sounds/', ln):
                                    insert_idx = li
                                    if safe_name in ln:
                                        found_in_section = True
                            if not found_in_section and insert_idx is not None:
                                new_inc = '// #include "sounds/%s" // %s (custom)' % (safe_name, safe_name.replace(".h", ""))
                                vf_lines.insert(insert_idx + 1, new_inc)
                                write_text(vf_path, "\n".join(vf_lines) + "\n")
                            # Now apply the sound choice to make it active (and deduplicate)
                            apply_sound_choices("vehicles/" + active_vehicle, {category_key: safe_name})
                            added_to_vehicle = True

                self.send_json({"ok": True, "filename": safe_name, "added_to_vehicle": added_to_vehicle})
            except Exception as e:
                self.send_json({"ok": False, "error": str(e)}, 500)
            return

        if self.path == "/save":
            try:
                payload = json.loads(body)
                for rel_path, fields in payload.items():
                    vehicle = fields.pop("__vehicle__", None)
                    if vehicle:
                        apply_vehicle(vehicle)
                        ensure_vehicle_backup(vehicle)

                    target_vehicle = get_current_vehicle()
                    if target_vehicle:
                        ensure_vehicle_backup(target_vehicle)

                    flag_changes = {}
                    value_changes = {}
                    sound_changes = {}

                    for name, info in fields.items():
                        kind = info.get("kind", "")
                        if kind == "flag":
                            flag_changes[name] = bool(info.get("enabled"))
                        elif kind == "bool_var":
                            value_changes[name] = str(info.get("value", "false"))
                        elif kind == "define_val":
                            flag_changes[name] = bool(info.get("enabled"))
                            value_changes[name] = str(info.get("value", ""))
                        elif kind == "text_var":
                            value_changes[name] = str(info.get("value", ""))
                        elif kind == "sound_choice" and name.startswith("__sound__"):
                            sound_key = name.replace("__sound__", "", 1)
                            sound_changes[sound_key] = str(info.get("value", "")).strip()

                    if flag_changes:
                        apply_changes(rel_path, flag_changes)
                    if value_changes:
                        apply_changes(rel_path, value_changes)
                    if rel_path.startswith("vehicles/") and sound_changes:
                        apply_sound_choices(rel_path, sound_changes)
                    if rel_path == "1_Vehicle.h" and target_vehicle and sound_changes:
                        apply_sound_choices("vehicles/" + target_vehicle, sound_changes)

                # Auto-fix: ensure the vehicle file is buildable after every save
                final_vehicle = get_current_vehicle()
                if final_vehicle:
                    auto_fixes = validate_and_fix_vehicle(final_vehicle)
                else:
                    auto_fixes = []

                self.send_json({"ok": True, "fixes": auto_fixes})
            except Exception as e:
                self.send_json({"ok": False, "error": str(e)}, 500)
            return

        if self.path == "/open_arduino_ide":
            try:
                sketch = os.path.join(SRC, "src.ino")
                ok, msg = open_arduino_ide(sketch)
                if ok:
                    self.send_json({"ok": True, "message": msg})
                else:
                    self.send_json({"ok": False, "error": msg}, 500)
            except Exception as e:
                self.send_json({"ok": False, "error": str(e)}, 500)
            return

        if self.path == "/run":
            try:
                req = json.loads(body)
                cmd_type = req.get("cmd", "build")

                # Activate the vehicle selected in the UI before building
                req_vehicle = req.get("vehicle", "").strip()
                if req_vehicle:
                    apply_vehicle(req_vehicle)

                cli = find_arduino_cli()
                if not cli:
                    self.send_json(
                        {"ok": False, "error": "Arduino IDE not found. Install Arduino IDE 2.x from https://www.arduino.cc/en/software — no other setup needed."},
                        500,
                    )
                    return

                sketch = os.path.join(SRC, "src.ino")
                if not os.path.isfile(sketch):
                    self.send_json({"ok": False, "error": "Sketch not found: %s" % sketch}, 500)
                    return

                # Safety: ensure 1_Vehicle.h has an active vehicle before building
                active = get_current_vehicle()
                if not active:
                    self.send_json({"ok": False, "error": "No vehicle is active in 1_Vehicle.h — select a vehicle first"}, 400)
                    return
                # Auto-fix any broken sound sections before building
                validate_and_fix_vehicle(active)

                if cmd_type == "flash" and not CONNECTED_PORT:
                    self.send_json({"ok": False, "error": "Board is not connected"}, 400)
                    return

                # Stream output from here
                self.send_response(200)
                self.send_header("Content-Type", "text/plain; charset=utf-8")
                self.send_header("Transfer-Encoding", "chunked")
                self.end_headers()

                def chunk(msg):
                    data = msg.encode("utf-8", errors="replace")
                    self.wfile.write(("%x\r\n" % len(data)).encode("ascii"))
                    self.wfile.write(data)
                    self.wfile.write(b"\r\n")
                    self.wfile.flush()

                # Auto-setup: install correct ESP32 core if needed
                chunk("Checking ESP32 toolchain...\n")
                if not ensure_esp32_core(cli, chunk):
                    chunk("\n--- DONE (exit 1) ---\n")
                    self.wfile.write(b"0\r\n\r\n")
                    self.wfile.flush()
                    return

                fqbn = "esp32:esp32:esp32"
                build_path = os.path.join(ROOT, "build")

                # Base compile command
                acli_cmd = [
                    cli, "compile",
                    "--fqbn", fqbn,
                    "--build-path", build_path,
                    "--build-property", "build.extra_flags=" + get_build_flags(),
                    "--build-property", "build.partitions=huge_app",
                ]

                # Add library paths
                for lib_path in get_library_paths():
                    acli_cmd += ["--library", lib_path]

                if cmd_type == "flash":
                    acli_cmd += ["--upload", "--port", CONNECTED_PORT]

                acli_cmd.append(SRC)  # sketch directory

                action = "Compiling + Uploading" if cmd_type == "flash" else "Compiling"
                chunk("%s with Arduino CLI...\n" % action)
                chunk("$ " + " ".join(acli_cmd) + "\n\n")

                proc = subprocess.Popen(
                    acli_cmd,
                    cwd=ROOT,
                    stdout=subprocess.PIPE,
                    stderr=subprocess.STDOUT,
                    text=True,
                    bufsize=1,
                    shell=(os.name == "nt"),
                )

                for line in proc.stdout:
                    try:
                        chunk(line)
                    except (BrokenPipeError, OSError):
                        break

                proc.wait()
                rc = proc.returncode
                status_line = "\n--- DONE (exit %d) ---\n" % rc
                chunk(status_line)
                self.wfile.write(b"0\r\n\r\n")
                self.wfile.flush()
            except Exception as e:
                try:
                    self.send_json({"ok": False, "error": str(e)}, 500)
                except Exception:
                    pass
            return

        self.send_response(404)
        self.end_headers()


def _get_local_ip():
    """Best-effort LAN IP for display purposes."""
    import socket
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        s.connect(("8.8.8.8", 80))
        ip = s.getsockname()[0]
        s.close()
        return ip
    except Exception:
        return "localhost"


class ThreadedHTTPServer(ThreadingMixIn, HTTPServer):
    daemon_threads = True

def main():
    server = ThreadedHTTPServer(("0.0.0.0", PORT), Handler)

    lan_ip = _get_local_ip()
    print("\nRC Engine Sound ESP32 Configurator")
    print("===================================")
    print("Local:   http://localhost:%d" % PORT)
    print("Network: http://%s:%d" % (lan_ip, PORT))
    print("\nOpen either URL in any browser (Windows, Mac, phone, etc.)")
    print("Ctrl+C to stop\n")

    try:
        server.serve_forever()
    except KeyboardInterrupt:
        print("\nStopped.")


if __name__ == "__main__":
    main()
