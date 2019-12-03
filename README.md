# This is an Arduino RC engine sound generator for ESP32.
It's based on the ATMega 328 version: https://github.com/TheDIYGuy999/Rc_Engine_Sound
and on bitlunis Halloween example: https://github.com/bitluni/MotionPumpkin

## Features:
- Many selectable engine sounds and startup sounds for cars and trucks (currently just one)
- Sound files up to 22'050Hz, 8bit, mono can be used
- Works best with a PAM8403 amplifier module, connected to pin 25, via a 10kOhm potentiometer
- The engine RPM is calculated according to RC signal input on pin 13 *** CAUTION, 3.3V max.! *** 10kOhm resistor recommended!
- Gear shifting is simulated in "curves.h"
- Use an ESP32 80MHz

## New in V 0.1:
- Runs on an ESP32 (80MHz)
- It's still quick and dirty, but works with the included V8 short course truck idle sound
- sound way better than the ATmega 328 version, thanks to  22'050Hz sampling rate (instead of 8'000Hz)



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
todo...

## Pictures (including optional additional filtering):
![](https://github.com/TheDIYGuy999/Rc_Engine_Sound_ESP32/blob/master/top.jpg)


2019, TheDIYGuy999
