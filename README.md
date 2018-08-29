<img align="right" src="https://github.com/davidhampgonsalves/CR14-emulator-for-CatGenie-120/raw/master/catgenie.gif"/>

# Arduino Based CR14 Emulator for CatGenie 120
Suppress the DRM that prevents soap cartridges from being refilled using an Arduino.

## Details
The CatGenie 120 bricks itself after a fixed number of washes until a new soap cartridge is inserted. To allow it to run without a cartridge or allow cartridge refilling I reverse engineered the verification process to allow it to be emulated and suppressed using an Arduino. Further details can be found in this [blog post](http://www.davidhampgonsalves.com/reverse-engineering-cat-genie-120-drm/).

## Usage
Once installed the CatGenie will function as before and alert you when the cartridge needs to be refiled. Then refill the cartridge and cycle the power of the unit to reset the usage count in the Arduino.

## Installation

### Arduino
This project can easily be flashed to any Arduino using [platform.io](https://platformio.org/) via: `pio run`. By default its setup to be used with an Arduino Uno but you can modify the platform.ini to support the other Arduino variants.

### Hardware
After the code has been uploaded to the Arduino its ready to replace the CR14 RFID reader. Disassemble the CatGenie and locate the CR14 board located next to the cartridge holder as pictured bellow.
<img src="https://github.com/davidhampgonsalves/CR14-emulator-for-CatGenie-120/raw/master/cr14.jpg" width=400px />

Unplug the CR14 and attach the I2C wires(white/black on my machine) to pins A4/A5 on the arduino and 5v power/ground(red/blue) to their respective pins.

This process is similar to installing a product called the CartridgeGenius which is well documented in this [video](https://www.youtube.com/watch?v=N8NyGfgZmQI).

Remember to keep the CR14 for future RFID based projects,

## Alternatives
If you want a less DIY option check out the [CartridgeGenius](http://cartridgegenius.com/) which I've read is a good solution.
