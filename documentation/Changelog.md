# Changelog

### Todo:
- Remove in vehicle files: XENON_LIGHTS, LED_INDICATORS, INDICATOR_DIR, doubleFlashBlueLight
- Add to EEPROM & select box: DOUBLE_CLUTCH and other transmission options

### 9.13.0-b7:
- Defines removed: HAZARDS_WHILE_5TH_WHEEL_UNLOCKED
- Variables added (adjustable on 192.168.4.1): hazardsWhile5thWheelUnlocked
- Neopixel animation mode selectable in dropdown menu on 192.168.4.1
- Defines removed: NO_CABLIGHTS, NO_FOGLIGHTS, XENON_LIGHTS, FLICKERING_WHILE_CRANKING, SEPARATE_FULL_BEAM
- Variables removed: INDICATOR_DIR
- Variables added (adjustable on 192.168.4.1): noCabLights, noFogLights, xenonLights, flickeringWileCranking, swap_L_R_indicators, separateFullBeam, flashingBlueLight
- First light settings available on 192.168.4.1
- Website with collapsible sections to keep it more organized
- Servo positions adjustable on 192.168.4.1
- Improved website look
- Scania dashboard splash logo by Frikkiebester
- Dashboard requires less processing power, no engine rpm lagging anymore, if Frevic dashboard is used
- More adjustable ESC parameters on 192.168.4.1
- Dashboard improved & optional dashboard from Frevic implemented. Select it in "dashboard.h" by uncommenting "#define FREVIC_DASHBOARD"
- New parameter "#define NEOPIXEL_ON_CH4" in "6_Lights.h" allows to connect the NEOPIXELS to servo CH 4 (BUS mode only, no 5th wheel servo in this case)
- ESC parameters are adjustable via 192.168.4.1
- in "#define WEMOS_D1_MINI_ESP32" mode only: Experimental Serial command interface for ESC parameters

### 9.12.0:
- Support for 36 Pin version removed
- Support for Wemos D1 Mini ESP32 added (#define WEMOS_D1_MINI_ESP32 in "0_generalSettings.h").  Allows to use the upcoming wireless trailer controller as modular sound controller.
- In BUS communication mode: steering servo signal output stays disabled until auto zero was successful. Important in IBUS mode to prevent servo "ghost moves"
- Short triple flash, if no valid input bus signal is detected, no reboot loop anymore
- Short double flash, if no valid battery is detected, no reboot loop anymore
- WiFi Tx power adjustable in "0_generalSettings.h"
- Short double flash, if battery error during controller start
- WiFi Tx power lowered = less noise in the speaker and longer battery life
- Tx power is displayed in the serial monitor
- WiFi based configuration website for trailer options added
- Settings files renamed, "adjustments" removed
- ESP32 trailer software moved to (the old verion without WiFi is still included): https://github.com/TheDIYGuy999/wirelessTrailerESP32web

### 9.11.0:
- LS3 Engine added, used in James Bond Spectre Defender: Defender62LS3.h
- Changes in ESP32 trailer sketch. Mac addresses which are not working (causing engine revving issues) were discovered thanks to FrikkieBester. Unfortunately I still don't know why.
- In 7_adjustmentsServos.h: NO_WINCH_DELAY option added, servo signal for wincn is changed immediately in this case

### 9.10.0:
- First quick and dirty STEAM_LOCOMOTIVE_MODE implementation (see vehicles/Adler.h). Note, that you need to disable VIRTUAL_3_SPEED
- Steam whistle on horn channel
- In 8_adjustmentsSound.h: #define NO_INDICATOR_SOUND option, if you don't want the tic - tac sound

### 9.9.1:
- Flysky SBUS should now be stable with #define EMBEDDED_SBUS. Please let me know, if not.
- BATTERY_PROTECTION: scaling bug with higher resistor values fixed.

### 9.9.0:
- New NEOPIXEL_UNION_JACK animation for Land Rover ;-)
- New MAX_POWER_MILLIAMPS setting for Neopixels. This and other optimizations is eliminating the flickering Neopixels issue
- 3_adjustmentsESC.h: globalAccelerationPercentage allows to adjust the acceleration, if vehicle setting is not OK
- 4_adjustmentsTransmission.h: lowRangePercentage allows to adjust the acceleration in low transmission (off road) range. Used for MODE1_SHIFTING and WPL 2 speed transmission
- Channel failsafe changed
- 2_adjustmentsRemote.h: External SBUS library not required anymore. See option #define EMBEDDED_SBUS. "sbusInverted" moved to remote configuration profiles, so you don't have to change it anymore
- SBUS still unstable with Flysky remotes! Especially in combination with WIRELESS_TRAILER. Use IBUS in this case! I'm thankful for hints about this bug. Differences: Micro RC is sending an SBUS packet every 14ms, Flysky every 7ms

### 9.8.0:
- Adjustable RECOVERY_HYSTERESIS for battery protection
- 8_adjustmentsSound.h (masterVolumeCrawlerThreshold) and 3_adjustmentsESC.h (crawlerEscRampTime): switchable crawler mode with just minimal inertia for gearbox protection. Allows to have a "show mode" and a "competition mode"
- 3_adjustmentsESC.h: Additional comments in battery low dischgarge section, according to neo2001

### 9.7.0:
- NEOPIXEL_KNIGHT_RIDER scanner optimized, synchrone with sound
- NEOPIXEL_BLUELIGHT: red & blue bluelight animation for fire trucks etc.
- NEOPIXEL_HIGHBEAM: Neopixel bar now also usable for full beam and headlight flasher. Also working in combination with NEOPIXEL_KNIGHT_RIDER & NEOPIXEL_BLUELIGHT
- "Audio2Header.html" tool now able to remove noise in silent passages (option "remove noise floor")
- ESC state machine hanging in case 1 or 3, if throttle trigger direction changed qickly: bug solved
- Engine volume raising, if throttle trigger direction changed qickly: bug solved
- Improved ESC_DEBUG messages
- 3_adjustmentsESC.h: options better organized, OVERDRIVE now also usable with VIRTUAL_3_SPEED. In this case, the 4th (overdrive) gear is engaged @ full throttle & high RPM in 3rd gear
- 2_adjustmentsRemote.h: CHANNEL_AVERAGING option (recommended, if you have issues with unstable channels)

### 9.6.0:
- New version numbering: mainVersion.subVersion.patch It is also displayed in Serial Monitor, so you always know, which version you have on your vehicle
- "Out of fuel" voice message in many languages in "3_adjustmentsESC.h"
- Number of battery cells in series indicated by number of beeps after powering up the controller (#define BATTERY_PROTECTION mode only). This lets you always know, if your battery was detected properly.
- Knight Rider KITT scanner Neopixel animation now with sound (select it as siren in vehicle file). Scanner is now only on, if siren is on.
- Optimized "#define SEMI_AUTOMATIC" mode: Transmission is now downshifting earlier, if brake is applied

### 9.5:
- Battery low discharge protection options added to "3_adjustmentsESC.h". Used in combination with ESC's without protection or internal RZ7886 motor driver
- Battery level is shown on LCD dashboard (bottom left instrument and central display).
- Experimental "#define HIGH_SLIPPINGPOINT" option in "4_adjustmentsTransmission.h"
- New Defender V8 with open pipes
- clutchEngagingPoint lowered from 100 to 10 in several automatic transmission vehicles. This is standard.

### 9.4:
- Ticking noise, caused by accidentally enabled WiFi removed
- SBUS baudrate can now be fine adjusted in order to solve unstable channel issues, causing accidentally triggered indicators etc.
- Make sure you are using the latest version of my SBUS library!

### 9.3:
- Support for RZ7886 motor driver IC instead of classic RC ESC. Suitable for example for WPL vehicles with 370 motor. See "3_adjustmentsESC.h"
- New "brakeMargin" setting in "3_adjustmentsESC.h"
- ESC scaling bug of v9.2 fixed

### 9.2:
- Now compatible with latest Espressif Systems ESP32 board definition 1.0.6 (problem was caused by haedlights conflicting with TX pin)
- ESC now controlled by mcpwm (as all other servo outputs) instead of statusLED
- "Knight Rider" scanner for 8 Neopixels
- "b33lz3bub" Austria cab light animation for 3 Neopixels

### 9.1:
- New Benford 3 Ton Dumper sound
- New profile for the stick based "Micro RC" DIY remote. Allows to use remote CH 2 & 4 for other stuff
- New "#define SEMI_AUTOMATIC" option allows to use the 3 position switch for other stuff. The manual transmission is switched automatically

### 9.0:
- New parameter #define AIRPLANE_MODE in vehicle file (see MesserschmittBf109.h) for airplanes: throttle range always 1000-2000, idle @ 1000, no clutch simulation. No ESC output support, connect it directly to ESC!
- Added Frikkies ramps ESC mode to the ESP32 trailer sketch.

### 8.9:
- New parameter in 10_adjustmentsTrailer.h: TRAILER_LIGHTS_TRAILER_PRESENCE_SWITCH_DEPENDENT switches wireless trailer lights off, if no trailer is detected on truck mounted optional trailer detection switch on pin 32. If yo uwant to use this switch, comment out THIRD_BRAKELIGHT in 6_adjustmentsLights.h

### 8.8:
- Enabled Neopixels not conflicting with steering servo speed anymore, only executed every 20ms
- More Neopixel options, but still experimental
- High engine RPM is not heavily slowing down steering servo movements anymore
- #define HAZARDS_WHILE_5TH_WHEEL_UNLOCKED enables hazards, if 5th wheel is unlocked

- New SMD board with more connectors and switch mode power supply added
- Trailer software v0.3 with trailer leg support and light on / off switch, switched by tracrors 5th wheel

### 8.7:
- Trailer 5th wheel unlocking by servo CH4 can now also be used in combination with my "Micro RC" remote. Use "#define MODE2_TRAILER_UNLOCKING" in "7_adjustmentsServos.h"
- Kenworth sounds reworked
- New option for flickering lights during engine cranking in "6_adjustmentsLights.h": "#define FLICKERING_WHILE_CRANKING"
- New option for old vehicles with unsynchronised transmissions in "4_adjustmentsTransmission.h": "#define DOUBLE_CLUTCH"
- New vehicles: Saurer 2DM Swiss truck, Peterbilt with Detroit Diesel 8V92, Kenworth with open pipes CAT 3408

### 8.6:
- Magirus Mercur 125 air cooled V6 Diesel truck sound

### 8.5:
- ESP-NOW is only transmitting, if a variable in the packet has changed. Advantages: longer battery life, ESP32 stays cooler, less noise in the speaker
- Support for multiple trailer slaves (intended for articulated cattle trailer)
- support for trailer servos (legs, ramps, rotating beacon control). Controlled via horn pot (if hazards on / 5th wheel uncoupled)

### 8.4:
- Trailer software version for ESP8266 added
- German Magirus Deutz 256 V8 truck sound added
- Master volume scale is now logarythmic
- New options in "vehicle.h": "#define RPM_DEPENDENT_KNOCK", "minKnockVolumePercentage", "knockStartRpm" allow to adjust the knock volume depending on Engine RPM as well, not just depending on throttle input. A lot of vehicle sounds were optimised, using these options.

### 8.3:
- ESP-NOW based 2.4GHz wireless trailer lights control. Enable it in "10_adjustmentsTrailer.h". You can use a 2nd controller in your trailer as a slave.
- Trailer software can be found in "trailer/wirelessTrailer" for now.

### 8.2:
- Visual Studio Code can now be used as an Arduino IDE replacement. Libraries and board definitions are downloaded and configured automatically in this case.
- Important: "Rc_Engine_Sound_ESP32.ino" is now named "src.ino" and located in the "src" folder. See chapter "software" above
- Please note, that the folder structure has changed. For example the "Audio2Header.html" sound conversion tool is now located in the "tools" folder.
- All vehicle- & sound-files are the same as before and fully compatible.

### 8.11:
- Serious bug fixed: PWM communication mode was broken (thanks to Benny for reporting this issue)! -> Semaphore for multi tasking access to "pwmBuf" variable added, RTC watchdog timer changed
- Dashboard LCD: Jumpy instruments needle start animation bug fixed
- Tested wit Flysky FS-i6X in PWM mode and Micro RC in SBUS mode. Both including LCD dashboard.

### 8.1:
- Variable tire squeal, depending on steering angle and speed as well as during heavy braking
- See vehicle "MercedesActrosV6.h" race truck
- enable it with "#define TIRE_SQUEAL"

### 8.0:
- Automatic RPM lowering, if not digging for 5s in excavator mode
- new "dashRotation" variable allows to mount the dashboard LCD upside down: 3 = normal, 1 = upside down
- master volume bug fixed
- new Mercedes V6 FIA race truck sound

### 7.9:
- Hydraulic load dependent engine RPM drop for excavator mode
- Bucket rattle sound, if bucket or dipper stick moved fast

### 7.8:
- Caterpillar 323 Excavator sounds added including hydraulic and track rattling sounds
- Support for hydlaulic excavators (hydraulic pump, hydraulic flow, track rattling sounds). Use #define FLYSKY_FS_I6S_EXCAVATOR profile for remote
- I'm using it for my KABOLITE K336. Connect RX to the IBUS SERVO connector on the KABOLITE receiver

### 7.7:
- Support for 0.96 inch 80x160 st7735 LCD dashboard (details and wiring see "9_adjustmentsDashboard.h", thanks to Gamadril)
- Support for 2812 Neopixel LED (details and wiring see "6_adjustmentsLights.h", thanks to Gamadril)
- Support for Graupner SUMD protocol (thanks to Gamadril)

### 7.6:
- Actros sound optimised. More bass.
- More light options in "6_adjustmentsLights.h". Xenon etc. This is more convenient than inside the vehicle files.
- Servo output compiling error solved
- Support for rotating beacons control in "7_adjustmentsServos.h": #define CH3_BEACON. Connect this beacon to CH3 (BUS mode only): https://www.ebay.ch/itm/303979210629
- New "headlightParkingBrightness" option in "6_adjustmentsLights.h" allows to use the headlights as parking lights.

### 7.5:
- Support for non linear throttle and steering curves (for more accurate control around center position). Use "EXPONENTIAL_THROTTLE" & "EXPONENTIAL_STEERING" in "2_adjustmentsRemote.h"
- Support for HOBBYWING Quicrun Fusion Motor / ESC combo. Use "#define QUICRUN_FUSION" in "3_adjustmentsESC.h"
- support for winch, connected to CH3 (BUS communication mode only). Use "#define MODE2_WINCH" in "7_adjustmentsServo.h" The mode 2 button is then used to switch between horn / siren sontrol and winch control via CH4. The winch is controlled by an old RC servo driver board. The speed and neutral settings are done using "CH3L", CH3C" and CH3R" positions.

### 7.4:
- In "2_adjustmentsRemote.h": New remote profile for Flysky GT5 / Reely GT6 EVO / Absima CR6P (thanks to BlackbirdXL1 for making this profile)
- In "2_adjustmentsRemote.h": New remote profile for MT-305. This remote comes with the RGT EX86100 crawler
- "NO_SIREN" option. Siren is not triggered in this case
- "maxPwmRpmPercentage" lowered from 400 to 380 to prevent controller from random crashing in PWM mlode
- Microseconds channel signal range limited to 1000 - 2000. Required for RGT MT-305 @ full throttle dual rate

### 7.3:
- New Jeep Wrangler 392 HEMI V8 (bombastic with a good bass speaker!)
- Optimised V8 vehicles, using the new "RPM_DEPENDENT_KNOCK" (in vehicle file) option. This makes the engine sound way more realistic and aggressive @ high RPM. Ideal for "muffler deleted" vehicles.
- New "SERVO_DEBUG" option makes the servo calibration process easier. See "7_adjustmentsServos.h" tab"
- "VIRTUAL_3_SPEED" not interfering with automatic transmissions anymore
- "OVERDRIVE" option for 6 speed automatic transmission as well, not just for 4 speed. It's recommendet to leave it turned on, unless you have a double clutch vehicle.

### 7.2:
- "NO_CABLIGHTS" bug fixed
- US_Firetruck memory problem fixed
- Channel auto zero calibration reworked. See serial monitor, if you have issues. Never use "channelAutoZero = true" for unconnected channels or channels without spring centering!
- Indicators used as a debugger: the amount of blinks shows you the channel, which is not in neutral position (outside of 1400 - 1600 microseconds, check channel trim settings). Indicators constantly on means, that no SBUS signal is detected (check "sbusInverted" true / false in "2_adjustmentsRemote.h").
- Unimog U1000 with engine brake for Hermann

### 7.1:
- Servo output for shifting transmission now usable in combination with simulated automatic transmission. Use it as an off road reducer, don't shift it while driving fast or @ high engine RPM!

### 7.0:
- In "6_adjustmentsLights.h" tab: NO_CABLIGHTS" will skip the cablights step in the light control sequence, if defined. Use it, if your vehicle does not have cab lights. More light brightness options
- New "8_adjustmentsSound.h" tab for global sound options
- Vehicle specific configurations in "7_adjustmentsServos.h" allow to store servo endpoints for multiple vehicles. Useful, if the sound controller is used as an SBUS decoder.
- New Chevy 468 big block V8
- New Land Rover V8 with open pipes
- Adjustable reverse speed for automatic transmissions: "automaticReverseAccelerationPercentage"
- New "OVERDRIVE" option in "4_adjustmentsTransmission.h" allows to lower the engine RPM in 4th automatic gear
- BlinkLeds.ino sketch for LED test in /tools

### 6.9:
- New "JAKEBRAKE_ENGINE_SLOWDOWN" option, used in Volvo FH trucks with open pipe. If defined, the engine RPM is lowered, using the jake brake, while upshifting under full load as well as when the throttle is released quickly in neutral. You can hear it in this video: https://www.youtube.com/watch?v=MU1iwzl33Zw&list=LL&index=4
- New volvo FH with open pipes
- New GMC Sierra V8 Pickup with 3 speed automatic transmission
- A lot of new horn melodies from nenno

### 6.81:
- Third brakelight bug fixed
- Урал375 renamed to URAL375 (Windows is not able to read a mix of western and cyrillic letters)

### 6.8:
- "STEERING_RAMP_TIME" allows "scale" steering servo movements (BUS communication mode only)

### 6.7:
- MAN KAT V8 Diesel German Bundeswehr military truck for Onkel_Tom

### 6.64:
- Known issues added

### 6.63:
- Trailer coupling / uncoupling sound triggering switch debounced, correct sounds should now be played every time
- Typos in trailer sound playback section fixed

### 6.62:
- ESC state machine not locking up anymore, if "escBrakeSteps" is bigger than "pulseNeutral". This allows to achieve a more aggressive brake deceleration.

### 6.61:
- "'couplingSampleCount' was not declared in this scope" bug fixed

### 6.6:
- Trailer coupling & uncoupling sounds now triggered by a normally open switch, connected between pin 32 and GND. Intended to use the original TAMIYA switch.
- New "#define THIRD_BRAKELIGHT" setting in the "6_adjustmentsLights.h" tab. Comment it out, if you want to use the switch above.
- A trailer coupler (5th wheel) servo can now be connectet to the "CH4" header" (not available in PWM communication mode). See note (3 in "adjustmentsRemote.xlsx"

### 6.5:
- macOS Big Sur 11.x compiling error fix explained. See:  [Big Sur Fix](BigSurFix.md)
- Trailer coupling & uncoupling sounds in preparation, not yet working

### 6.4:
- CH1 & CH2 input pins can be used as PWM output for the steering servo and for the shifting servo. The outputs are active, if a BUS communication mode (SBUS, IBUS or PPM) is selected. This allows to use vers small "BUS-only" receivers. In this case, the servos are connected to the sound controller instead.
- Additional Servo adjustments parameters in tab "7-adjustmentsServo.h" added:
   - "SERVO_FREQUENCY" 50Hz is standard, some servos run smoother @ 100Hz.
   - Servo limits: left, center, right (allows to adjust both end points as well as the center point separately

### 6.3:
- Additional ESC adjustments parameters in tab "3-adjustmentsESC.h" added. Allows to optimise the HOBBYWING 1060 ESC, even if it has no internal adjustments parameters:
   - "escTakeoffPunch" adds more power, if accelerating from zero. Used, if your "clutch" is slipping too much while taking off.
   - "escReversePlus" allows to make the reverse speed faster (only, if "escPulseSpan" is more than 500)
- Additional "6_adjustmentsLights.h" tab:
   - "NO_FOGLIGHTS" will skip the foglights step in the light control sequence, if defined. Use it, if your vehicle does not have fog lights
   - "sideLightsBrightness" allows to adjust the brightness" of your parking lights / side markers
- New vehicle configurations like Diesel and petrol Landcruisers, generic 6 Cylinder Diesel (optimised for smaller speakers)

### 6.2:
- GAZ 66 redefinition bug fixed
- SMD version BOM error fixed
- Instructions PDF for ordering of pre assembled boards added
- STL data for SMD version bottom shell included
- Wastegate not triggered anymore while downshifting

### 6.1:
- SMD version tested
- "INDICATOR_SIDE_MARKERS" option: indicators are always on with dimmed brightness, if side markeers / parking light is on. This is often used in US trucks. See "KenworthW900ACAT3408.h" vehicle configuration
- You need to install the latest version of the "statusLED" library: https://github.com/TheDIYGuy999/statusLED
- Experimental new throttle array for heavy trucks in "curves.h" you need to comment / uncomment it manually for now.

### 6.0:
- Pig sound for Onkel_Tom ;-)
- Kenwworth W900A with CAT3408 made more aggressive (more bass) & alternative profile
- SMD version preview

### 5.9:
- LED PWM frequency is now 20kHz instead of 500 Hz. No PWM noise anymore in the speakers
- New "SEPARATE_FULL_BEAM" option. Wire your high beam lamps to the roof lights pin in this case
- New "INDICATOR_SIDE_MARKERS" option. Indicators are used as side markers as well in this mode

### 5.8:
- MAN TGX 680 vehicle profile
- SMD board v1.1, incl. Gerbers, BOM & Pick and Place file (still untestet)

### 5.7:
- Experimental support for virtual 16 speed sequential transmission "VIRTUAL_16_SPEED_SEQUENTIAL"
- New, alternative URAL-375D vehicle preset
- Indicators are now swichting on and off "soft" like incandescent bulbs, using an on and off ramp, if not defined "LED_INDICATORS" in the vehicle preset file. You need to use the latest statusLED update: https://github.com/TheDIYGuy999/statusLED
- New, untested SMD board version included

### 5.6:
- Horn & siren with variable length (as long as triggered). "Audio2Header.html" tool changed to add the required loop area (you need to adjust it manually) in the sound files.
- Horn & siren can play @ the same time
- New US fire truck (with CAT3408 V8 & Allison 6 speed automatic transmission) vehicle preset (use it to test the new variable length sounds)

### 5.5 (another big update):
- Way more flexible channel assignment, can easily be configured for my "Micro RC" remote, for a FLSKY FS-i6X or others. New "2_adjustmentsRemote.h" config file. You don't have to change the main code anymore in order to assign your channels. For details refer to "adjustmentsRemote.xlsx"
- New, state machine controlled lights control, if "#define AUTO_LIGHTS" commented out. Otherwise lights are controlled by the engine state
- New, optional xenon bulb ignition flash for headlights, if "#define XENON_LIGHTS" defined
- New option "#define ESC_DIR" allows to change the motor spinning direction in software
- New option "#define VIRTUAL_3_SPEED" lets you use the 3 position switch controlled 3 speed transmission, even if it doesn't exist ;-)
- Engine can be started manually by CH5 or automatically with the throttle stick (or the "momentary1" button on the "Micro RC" remote). Depending on setting "#define AUTO_ENGINE_ON_OFF"
- Serial communication mode for "Micro RC" remote removed (replaced with SBUS)
- Actions triggering rewritten, using updated rcTrigger library. You have to use the latest version: https://github.com/TheDIYGuy999/rcTrigger
- Added the missing "shifting" file type & german dropdown translations in "Audio2Header.html"
- Bug in Mack Super Liner configuration fixed
- IBUS interface tested, works fine, but "MAX_RPM_PERCENTAGE" > 350 will crash the ESP32. I recommend to use SBUS instead, because current Flysky firmware can generate SBUS signals as well.
- "MAX_RPM_PERCENTAGE" automatically limited according to communication mode. Required to prevent the controller from crashing (see above)
- In PWM mode, we can now read 6 channels
- PPM interface moved to RX pin, pins "35" & "PPM" are now used for PWM channels 5 & 6
- Bug fixed: wasteghate not triggered anymore while releasing brake rapidly
- Two-stage triggering for bluelight and siren, so we can use the bluelight separately
- Changing between jake braking and the normal engine sound is locked unless the current sample is looping back. This eliminates erratic klicking noises, if enabling or disabling the jake brake!
- To prevent issues with the mode buttons, you should install the latest software v3.5 on the "Micro RC" receiver
- If you use your own vehicle preset, you have to edit them, so that they use the same structure as the pre made ones. example: remove "const" in front of "uint32_t MAX_RPM_PERCENTAGE"
- New jake brake sounds for: Actros, URAL 4320, URAL 375D, Tatra 813 & M35. New dixie horn.

### 5.4:
- New: Volvo FH16 750, Dodge RAM 2500 with Cummins 12V
- "R6" knock pattern for inline 6 engines

### 5.3:
- PWM RC signal processing completely rewritten. It's now using the interrupt based ESP32 "rmt" core function rather than pulseRead(). Thanks to croky_b for the hint! No "MAX_RPM_PERCENTAGE" limitations anymore in PWM mode. 400% can now be used as well.
- PWM CH5 is now accessible on pin 35. This means, that the CH5 functions can also be used in PWM mode!
- Gear manual shifting sound not triggered anymore in automatic or doubleClutch mode. No need to disable it in automatic vehicles anymore.
- URAL4320, Unimog 1000 optimised
- New: Kirovets K700 V12 Diesel monster tractor with extreme turbo sound!

### 5.2 (a big update):
- New, comfortable .wato .h sound file converting tool ("Audio2Header.html" included). Based on bitluni's work
- Experimental Flysky IBUS support (see comunications settings, untested, you need to install this library: https://github.com/bmellink/IBusBM)
- New vehicle configuration structure: separate .h preset files for vehicles. Select them in "Adjustments.h". Makes vehicle selection a lot easier.
- Vehicle presets complely reworked. Most of them are now including a resound, which is seamlessly mixed together with the idle sound. Huge sound quality improvements!
- All vehicle specific presets and sound files are now located in "/vehicles"
- New vehicles: Defender Td5, VW Käfer, Actros 1863, SCANIA V8 trucks, URAL 375D, Jaguar XJS, MGB GT V8, Ford Powerstroke 7.3l V8 Diesel
- Idle and resounds are now throttle dependent es well, not just RPM dependent. Adjustable with "fullThrottleVolumePercentage". Useful, if you don't want to use the separate knock sound.
- "TOP_SPEED_MULTIPLIER" renamed to "MAX_RPM_PERCENTAGE", Max. engine RPM now adjustable in % of idle RPM instead of integer multiplier. Allows to do finer adjustments.
- "ADAPTIVE_KNOCK_VOLUME" renamed to "V8", added "V2" (these are ignition volume patterns for different engine types)
- "CATERPILLAR_MODE" renamed to "TRACKED_MODE" (for track based vehicles with dual ESC)
- new "SUPER_SLOW" option for very heavy, slow responding engines like locomotive diesels (see UnionPacific2002.h)
- "NumberOfAutomaticGears" setting moved from "curves.h" to "Adjustments.h"
- Gear ratio of 6 speed automatic transmission 1st gear changed, was too high
- Experimental pulseRead function (using interrupts) for PWM mode (disabled)
- Less aggressive RPM rise while downshifting the manual transmission
- Removed experimental "Multi slot knock samples", takes too much processing power in interrupt and does not sound good

### 5.1:
- Compiler warnings issue solved
- rcTrigger function moved to Core 1 (current Arduino IDE required, 1.8.7 does not work!)
- a lot of new functions (see below, triggered in "void rcTrigger()")
- Jake brake sound added (CH5, SBUS only) Uncomment //#define JAKE_BRAKE_SOUND, if you want to use it
- Headlight flasher and high / low beam switching added (CH5, SBUS only)
- Switchable master volume: indoor and outdoor mode (CH5, SBUS only)
- Switchable, virtual neutral for transmission, allows to rethe engine while standing still! (CH5, SBUS only)
- Kenworth W900A example configurations in Caterpillar 3408 (V8 4 stroke) and Detroit Diesel 8V71 (V8 2 stroke) version
- note, that you have to use the latest version of the rcTrigger library

### 5.01:
- Configuration mistake solved: "ADAPTIVE_KNOCK_VOLUME" (for V8) was not enabled in "Cat3408Settings" and *Master" setting"
- CAT 3408 Peterbilt added

### 5.0:
- The already existing "REV_SOUND" option is now fading over between the idle sound and the resound. This provides a very realistic behavior for heavy truck engines. Example see "Cat3408Settings", "Messerschmitt Bf109", "Unimog U1000". The fading threshold can be adjusted with the "revSwitchPoint" variable
- Optimized throttle fader for more realistic sound while gear shifting
- Optimised Wastegate triggering
- Optimised and new sound examples: Unimog U 1000 Turbo, Caterpillar 3408 (for King Hauler)
- Note, that only the "Master" settings example is listing all the .h files. Use this as a base for own settings

### 4.9:
- Ignition knock sound is now triggered by the optional revving sound as well
- Sounds and configuration example for Messerschmitt Bf 109 added (first example, which uses resound)

### 4.82:
- Missing .h files for the Ural 2 setting added
- Once again thanks to MAN Tgx for pointing out this issue

### 4.81:
- Bugfix: Several file names contained "Scanıa" (with Turkish i) instead of "Scania". This appears to be a problem under Windows but works fine under macOS
- Example configurations and .h files were reworked accordingly
- Thanks to MAN Tgx on the German RC Modellbau Portal for reporting this bug: https://www.rc-modellbau-portal.de/index.php?threads/esp32-arduino-rc-sound-und-licht-controller.7183/page-14#post-229253

### 4.8:
- Throttle fader for more natural knock behavior during take off
- Improved wiring and adjustments instructions down below
- New insanely loud supercharger sound option
- Example see "JeepCherokeeTrackhawkSettings" (use 6 gears in "curves.h)

### 4.7:
- Experimental support for my new rcTrigger library added
- Allows to trigger multiple latching and non latching actions per analog channel
- Note, that you have to install this library: https://github.com/TheDIYGuy999/rcTrigger

### 4.6:
- 75mm tank cannon fire sound and flash added
- Triggered via CH4
- Flash is using "Beacon 1" output
- Example settings: CATERPILLAR_MODE

### 4.5:
- CATERPILLAR_MODE (dual throttle input on CH2 & CH3 for tanks, diggers, bulldozers etc.
- No ESC control support in this mode! Wire two crawler type ESC (with direct brake) in partallel with CH2 & CH3 inputs, use dual headers)
- Example settings: CATERPILLAR_MODE_Settings (copy and paste it in "Adjustments.h")
- Also working in SBUS mode (allows to use additional sounds)
- New V12 tank sound from JPS952

### 4.4:
- Jeep Grand Cherokee Trackhawk added. Speaker with good bass reproduction needed!
- DAF XF truck added
- Adaptive knock volume added (the first knock per engine cycle will be louder, if defined "ADAPTIVE_KNOCK_VOLUME", adjust volume for others with "dieselKnockAdaptiveVolumePercentage" (see DAF XF settings)
- "Settings" files cleaned up

### 4.3:
- Simulated sequential double clutch transmission added. Variable "doubleClutch" in "Adjustments.h". See Ferrari setting (use 6 gears in "curves.h")
- Automatic transmission shifting strategy is now more robust, no oscillation anymore. See Chevy Nova V8 setting (use 3 gears in "curves.h")
- Knock sound for Chevy Nova V8 added. Sounds just great in combination with the 3 speed automatic transmission!

### 4.2:
- Dynamic, adaptive shift points for automatic transmission, depending on engine load allows economic driving
- Kick down detection will shift back, if instant full throttle is applied
- Replaced "sawtooth" arrays for automatic gear ratios with three one dimensional arrays for 3, 4 or 6 gears
- Reverse detection, only one gear in this case
- Gear ratios according to Allison datasheet (adjust reverse gear ratio according to your ESC reverse top speed)
- PROTOTYPE_36-PIN renamed to PROTOTYPE_36 to prevent compiler warnings
- Settings profiles for Ferrari & trophy truck with 6 speed automatic transmission & Chevy Nova with 3 speed automatic transmission added

### 4.12:
- Further automatic transmission fine tuning after big test drive with the Actros

### 4.11:
- Automatic transmission optimised in 1/14 scale Actros truck with Hobbywing 1080 ESC

### 4.1:
- Inspired by Wombii's fork of this repo: optimised simulated automatic transmission: load dependent "torque converter slip" added, optimised "4 speed sawtooth array", which is simulating the shifting points. Now we have 4 gears instead of 3, but you still can uncomment the 3 speed version array and use it instead. I recommend to use "automatic = true" in "Adjustments.h", if your RC vehicle does not have a real shifting transmission. My solution is way more simplistic than Wombii's, but still sounding good. Example settings see Land Rover and fire truck
- Harley Davidson optimised

### 4.0:
- Failsafe for SBUS now implemented and tested in the King Hauler build. The vehicle will do a controlled stop, if the SBUS wire is disconnected
- The SBUS failsafe flag from the receiver should in theory do the same, but I was not yet able to test it

### 3.9:
- Initial SBUS support on pin 36 "VP". Tested in combination with my "Micro RC" receiver (SW v3.4). Only one single signal wire required!
- It's working fine, but failsafe is not yet ready. Use it at your own risk!
- Use variables "SBUS_COMMUNICATION" & "sbusInverted" (changing signal polarity without an additional transistor) in "Adjustments.h" for configuration
- if you want to change the channel assignment, do it in readSbusCommands()
- Throttle overflow bug solved
- New, optimized Detroit Diesel mix for the KingHauler see "KingHaulerSettings2"

### 3.8:
- Experimental support for automatic transmission simulation. Use "automatic = true" to activate it
- Eagle schematic now including .brd PCB file -> upload it on www.oshpark.com to order your boards easily

### 3.7:
- "PROTOTYPE_36-PIN" allows switching headlights pin between D0 & TX0 pin. Required for the change to a 30 pin ESP32 board, which will be used on the final PCB.
- On the final PCB, the ESP32 will be powered from the ESC, the amplifier, the shaker and the LED from the on board 5regulator. This requires to remove the popping noise, which is generated, when the DAC offset of 128 is activated. So I added a fader for the DAC offset.

### 3.61:
- Tatra 813 V12 8x8 sound optimised
- New cooling fan & knock sounds

### 3.6:
- engine cooling fan sound added
- Scania 50 ton truck added
- Air cooled Tatra 813 V12 8x8 truck added

### 3.5:
- "dieselKnockStartPoint" variable allows to adjust the point where the volume of the knock sound is starting to increase
- Diesel knock sound triggering bug fixed
- Mercedes Actros & Harley Davidson sounds added
- Example files for vehicle specific settings added

### 3.4:
- Experimental support for separate engine revving sound. See "REV_SOUND" in Adjustments.h (don't use it)
- New Diesel ignition "knock" sound (fixed sample rate), played in synch with the engine sound (variable sample rate). This results in a way more realistic Diesel sound, because the ignition "knock" does not vary in pitch.
- engine sound is now mixed together from up to 4 sounds: idle, turbo, wastegate, Diesel knock. All of them are variable in volume, depending on throttle position or engine RPM. You can adjust everything, using a lot of variables in "Adjustments,h". The result is a very realistic engine sound. Allows to use an engine RPM range of 3 instead of 2 without sounding strange
- New squeaky brake sound
- Up to 11 sounds are now played in parallel!

### 3.3:
- Dimmed headlights during engine start
- International DT-466 sounds added
- New SCANIA V8 added
- Brake sound moved to the fixed sample rate playback interrupt
- Cleaned up playback interrupt functions
- DAC offset 128 moved to dacWrite()
- Brake sounds don't have to start & end @ -128 anymore
- Parking brake sound added
- Pneumatic shifting sound added

### 3.2:
- Added the new "Wastegate" sound. It is triggered, if the throttle is dropped rapidly
- Added Unimog U 1000, uses the new Wastegate sound
- Added AM General M35 truck

### 3.1:
- Added a new, pretty realistic SCANIA R620 "Straight Pipe" engine sound (ScaniaR620Uphill2.h). Pure goose flesh!
- Detroit diesel sounds with more bass added
- A speaker with good bass reproduction is required!

### 3.0:
- Ramping ESC signal faster below "clutchEngagingPoint" to prevent clutch from excessive slipping
- Solved a 2nd engine rpm bug during braking

### 2.91:
- Engine sound was slowing down too much during braking. This bug is now solved
- No turbo sound while braking: Bug solved
- King Hauler parameters fine adjusted
- "clutchClosingPoint" renamed to "clutchEngagingPoint"

### 2.9:
- "shiftingAutoThrottle" is altering throttle for synchronizing while shifting the TAMIYA 3 speed transmission -> Gearbox is protected, extreme realism in engine sound!
- More inertia added, engine idle volume lowered to 40%

### 2.8:
- "clutchClosingPoint" configuration variable added. Prevents engine sound from "rubber band effect". Engine sound is in synch (clutch engaged) with ESC power above this value
- Virtual clutch is also disengaging during gear shifting, allowing the engine rpm to settle down during shifting
- Acceleration speed now variable, depending on throttle position
- Added way more inertia to make vehicle behavior even more realistic
- Brake has now much finer granularity

### 2.7:
- Hornblasters train horns added
- Horns are now louder
- Trash removed
- Acceleratoin & deceleration now shifting transmission gear dependent

### 2.6:
- Indicator activation point adjustable with "indicatorOn"
- Norwegian siren, new air brake sound
- Door noise
- sound files moved to /sounds
- optimized bluelight patterns, using the new, optional "delay" variable. Latest statusLED library update required: https://github.com/TheDIYGuy999/statusLED

### 2.5:
- 3rd brake light on pin 32 added
- "escPulseSpan" > 1400 now working properly. Allows to accelerate and decelerate even smoother as well as limiting the top speed to a realistic level

### 2.4:
- Bug fixed: direct transition from state "braking backwards" to "driving backwards" now working
- failsafe function for serial signal added. Emergency brake is triggered in case of RC signal loss (serial & PWM mode only, PPM will follow)

### 2.3:
- Reversing light, reversing beep and brake light behaviour changed in order to interact correctly with the new vehicle mass simulation
- ESC is locked, if engine is not running ;-)
- Air brake sound now triggered correctly, as soon as the brake pedal is released
- I'm really amazed about this update! The vecihle almost behaves like a real truck now.

### 2.2:
- Support for crawler ESC (with direct brake) on pin 33. Vehicle mass simulation with proportional brake! Tested in Tamiya King Hauler (REELY Model No H0050 ESC) and working fine, but use it at your own risk! If you want to test it, connect ESC to pin 33 of the ESP32 instead of the receiver output.
- LED pins rearranged  to fit my board layout (you can change it in the pin assignment section)
- Scania R620 sound added

### 2.1:
- Additional turbo sound (playing in parallel with engine sound), volume and throttle dependent volume adjustable
- Caterpillar 3406B sound added
- VW Beetle sound added
- Indicator direction adjustable with "INDICATOR_DIR" = "true" or "false"
- Shaker rpm now adjustable for engine start, idle, full throttle & shutdown

### 2.0:
- Swiss post bus horn moved to category "sirens" ---> Replace existing .h file!
- Experimental ESC output (do not use it). Latest statusLED library update required: https://github.com/TheDIYGuy999/statusLED
- Mack truck sounds added

### 1.9:
- Kenworth W900 added
- Initial PPM signal input on pin 34 added. For now usable for steering and throttle signal. Uncomment "#define PPM_COMMUNICATION", if you want to use it.

### 1.8:
- LED pins changed for trailer light driver
- Horn sounds, siren sounds & additional sounds variable names changed! ---> Replace them all!
- Added more horns
- Added indicator (blinker) ticking sound
- Added independent volume adjustment for every sound
- Engine sound volume is now throttle dependent (adjustable)

### 1.7:
- Added shaker motor control on pin 23. Simulates engine vibration. IRLZ44N Mosfet and freewheel diode 1N5819 required
- Shaker power can be adjusted with the variables "shakerMin" & "shakerMax" in Adjustments.h
- Driver schematic down below
- I'm using a shaker motor from the G.T.Power kit 103061 (this controller is crap, but the accessories are useful)
- Foglight and side light pins moved to pins 18 & 19

### 1.6:
- Servo input channels rearranged to match the pinout of the "Micro RC" receiver (you can change it in the pin assignment section)
- Cleaned up the code a bit
- An updated version (16 timers) of my "statusLED" library is required: https://github.com/TheDIYGuy999/statusLED
- Added experimental support for serial communication with my "Micro RC" receiver: https://github.com/TheDIYGuy999/Micro_RC_Receiver
- Uncomment "#define SERIAL_COMMUNICATION" in Adjustments.h
- Connect pin 36 "VP" to pin "TXO" of the receiver (disable "TXO_momentary1" & "TXO_toggle1" in vehicleConfig.h)
- The "RC Servo Signals" are not required in this mode!
- Added Scania R500 V8 sounds

### 1.51:
- Solved issue: redefinition of 'const unsigned int sampleRate'

### 1.5:
- More Detroit Diesels added

### 1.4:
- Indicator (turn signal) support added
- RC signals now handled as arrays in order to simplify auto zero calibration of multiple channels

### 1.3:
- Irish fire truck horn
- Double flash blue lights option
- US fire truck air horn and air siren

### 1.2:
- Light functions added: reversing light, beacons (blue light), headlights, taillights with integrated brake lights
- My "statusLED" library is required: https://github.com/TheDIYGuy999/statusLED

### 1.1:
- Truck reversing beep sound added
- Can play in parallel with other additional sound and with variable speed engine sound!

### 1.0:
- Engine now slowing down, if switched off, not just fading out
- Fader bug fixed, now working smoothly
- HG P408 sounds added

### 0.9:
- Humvee Diesel engine sound added
- Truck pneumatic brake release sound added

### 0.8:
- volume offset bug was back and is fixed again!
- improved sound quality of Union Pacific 2002
- Detroit Diesel truck added
- Sine test tones added
- M2 sound added

### 0.7:
- fixed auto throttle zero calibration bug
- fixed "gap" bug, if sounds were triggered in PWM mode
- PWM mode now allows to trigger 2 sounds

### 0.6:
- fixed slow engine RPM response: The RC signal pulsewidth is now only read every 100ms, so much more processing time is available for other things

### 0.5:
- added idle sound volume adjustment variable "idleVolumePercentage" in "Adjustments.h"
- added Union Pacific 2002 SD70M 16 cylinder Diesel locomotive sounds
- added Jaguar XJS HE car sound (bad quality)
- added "pwmHornTrigger" variable in Adjustments.h", which allows to trigger the horn, using a PWM signal. Allows to use standard receivers

### 0.4:
- volume offset bug fixed
- added the typical Swisss post bus horn, an American police siren, a European Feuerwehr Martinshorn and a car horn
- added the varible "shifted", which allows to select between linear and shifted engine rpm response
- added schematic

### 0.3:
- Engine start / stop implemented, including starter sounds
- Three different start sounds available
- Horn implemented. It is played on pin 26 (see wiring instructions in code) triggered by switch on input 12
- Two different horn sounds available
- The horn can play in parallel with the variable speed engine sound!
- For easier access, the "settings.h" file was renamed in "Adjustments.h"

### 0.2:
- Many new 22'050Hz, 8bit sound files generated, mostly the same as in the ATmega 328 version, but in better quality :-)

### 0.1:
- Runs on an ESP32 (240MHz)
- It's still quick and dirty, but works with the included V8 short course truck idle sound
- sounds way better than the ATmega 328 version, thanks to  22'050Hz sampling rate (instead of 8'000Hz)
