# Smart Home - ESP code
This repository contains code for ESP32s which are used in a smart home application. There exist the following folders:
* `coffee-machine-esp` contains code for an ESP32 that is mounted inside a coffee machine in order to remote control it.
* `sensor-box-esp` contains code for an ESP32 that is connected to some environment sensors (temperature, humidity, pressure, illumination, air quality) for tracking the conditions inside the room.
* `esp-general` contains code that is used in more than one application for reducing code redundancy. Because the Arduino IDE cannot include code from parent folders, there is a python script `setup_arduino_files.py` which copies all necessary files from different folders into one folder which can then be opened in the Arduino IDE and uploaded to the ESPs. These files and folders are created inside the folder `arduino-upload`. Note that all files in this folder are overwritten when executing `setup_arduino_files.py`, so don't save any new work in the `arduino-upload` folder. 

The code for the server is not included in this repository.