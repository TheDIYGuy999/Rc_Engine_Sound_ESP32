# This document explains how to solve the macOS Big Sur ESP32 compiling error

On a vanilla ESP32 board definition installation on mac OS Big Sur, the ESP32 throws the following (or similar) Python compiling error:
```
Traceback (most recent call last):
  File "esptool.py", line 57, in <module>
  File "/Library/Python/2.7/site-packages/PyInstaller/loader/pyimod03_importers.py", line 389, in load_module
  File "serial/tools/list_ports.py", line 29, in <module>
  File "/Library/Python/2.7/site-packages/PyInstaller/loader/pyimod03_importers.py", line 389, in load_module
  File "serial/tools/list_ports_posix.py", line 31, in <module>
  File "/Library/Python/2.7/site-packages/PyInstaller/loader/pyimod03_importers.py", line 389, in load_module
  File "serial/tools/list_ports_osx.py", line 32, in <module>
ValueError: dlsym(RTLD_DEFAULT, kIOMasterPortDefault): symbol not found
Failed to execute script esptool
exit status 255
Error compiling for board ESP32 Dev Module.
```
The original fix comes from here:
https://forum.arduino.cc/index.php?topic=699989.0

I tried to make it a bit easier to understand. It was tested on the new Mac mini M1 with ARM chip, but it should work as well on Intel based systems.

### 1. Open Finder, select "Go to > Library"  in dropdown menu (hidden folder, only visible, if "alt" is pressed

copy "esptool.py" from:

/Users/XXXXX/Library/Arduino15/packages/esp32/hardware/esp32/1.0.4/tools

to:

/Users/XXXXX/Library/Arduino15/packages/esp32/tools/esptool_py/2.6.1

### 2. Run the following commands in Terminal:

Enable superuser permissions (you need to enter your user password):
```
sudo su
```

Change directory:
```
cd /Users/XXXXX/Library/Arduino15/packages/esp32/tools/esptool_py/2.6.1
```

Set permissions:
```
chmod +x esptool.py
```

### 3. Back in finder, change the following in the file:
/Users/XXXXX/Library/Arduino15/packages/esp32/hardware/esp32/1.0.4/platform.txt

from:

tools.esptool_py.cmd=esptool

to:

tools.esptool_py.cmd=esptool.py

### 4. Safe the file, restart Arduino IDE


2021, TheDIYGuy999
