# This is an Arduino RC engine sound generator for ESP32
It's based on the ATmega 328 version: https://github.com/TheDIYGuy999/Rc_Engine_Sound
and on bitlunis Halloween example: https://github.com/bitluni/MotionPumpkin

## Features:
- Many selectable engine sounds and startup sounds for cars and trucks
- Sound files up to 22'050Hz, 8bit, mono can be used
- Works best with a PAM8403 amplifier module, connected to pin 25, via a 10kOhm potentiometer
- The horn is played on pin 26 (see wiring instructions in code)
- The engine RPM is calculated according to RC signal input on pin 13 *** CAUTION, 3.3V max.! *** 10kOhm resistor recommended!
- Gear shifting is simulated in "curves.h"
- Use an ESP32, CPU frequency must be set to 240MHz

## New in V 0.1:
- Runs on an ESP32 (240MHz)
- It's still quick and dirty, but works with the included V8 short course truck idle sound
- sounds way better than the ATmega 328 version, thanks to  22'050Hz sampling rate (instead of 8'000Hz)

## New in V 0.2:
- Many new 22'050Hz, 8bit sound files generated, mostly the same as in the ATmega 328 version, but in better quality :-)

## New in V 0.3:
- Engine start / stop implemented, including starter sounds
- Three different start sounds available
- Horn implemented. It is played on pin 26 (see wiring instructions in code) triggered by switch on input 12
- Two different horn sounds available
- The horn can play in parallel with the variable speed engine sound!
- For easier access, the "settings.h" file was renamed in "Adjustments.h"

## New in V 0.4:
- volume offset bug fixed
- added the typical Swisss post bus horn, an American police siren, a European Feuerwehr Martinshorn and a car horn
- added the varible "shifted", which allows to select between linear and shifted engine rpm response
- added schematic

## New in V 0.5:
- added idle sound volume adjustment variable "idleVolumePercentage" in "Adjustments.h"
- added Union Pacific 2002 SD70M 16 cylinder Diesel locomotive sounds
- added Jaguar XJS HE car sound (bad quality)
- added "pwmHornTrigger" variable in Adjustments.h", which allows to trigger the horn, using a PWM signal. Allows to use standard receivers

## New in V 0.6:
- fixed slow engine RPM response: The RC signal pulsewidth is now only read every 100ms, so much more processing time is available for other things

## New in V 0.7:
- fixed auto throttle zero calibration bug
- fixed "gap" bug, if sounds were triggered in PWM mode
- PWM mode now allows to trigger 2 sounds

## New in V 0.8:
- volume offset bug was back and is fixed again!
- improved sound quality of Union Pacific 2002
- Detroit Diesel truck added
- Sine test tones added
- M2 sound added

## New in V 0.9:
- Humvee Diesel engine sound added
- Truck pneumatic brake release sound added

## New in V 1.0:
- Engine now slowing down, if switched off, not just fading out
- Fader bug fixed, now working smoothly
- HG P408 sounds added

## New in V 1.1:
- Truck reversing beep sound added
- Can play in parallel with other additional sound and with variable speed engine sound!

## On the todo list:
- Light functions for indicators, beacons, etc.

## Ho to create new sound arrays:

### Audacity:
- Import the sound file you want in Audacity
- Convert it to mono, if needed
- on the bottom left, select project frequency 22'050Hz
- cut the sound to one engine cycle. Zoom in to find the exact zero crossing
- adjust the volume, so that the entire range is used
- select > export audio > WAV > 8-bit-PCM
- note, that the files should be as short as possible: search for a repeating sound pattern and cut it to this length

### Convert the wav file with bitlunis converting tool:
- open https://bitluni.net/wp-content/uploads/2018/01/Audio2Header.html
- select the generated wav file from Audacity
- select "normalize"
- select "resample" and the frequency (22'050 recommended)
- a .h file is generated and downloaded
- move the .h file to your sketch directory

### Processing the new header file with your sound:
- include this .h file in "settings.h"

### Compiling the new sketch:
- compile and upload the sketch in Arduino IDE
- the new engine should now run...

## Schematic:
![](https://github.com/TheDIYGuy999/Rc_Engine_Sound_ESP32/blob/master/wiring.jpg)

## Pictures:
![](https://github.com/TheDIYGuy999/Rc_Engine_Sound_ESP32/blob/master/top.jpg)


2019, TheDIYGuy999
