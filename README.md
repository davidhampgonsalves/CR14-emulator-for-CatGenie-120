<img align="right" src="https://github.com/davidhampgonsalves/CR14-emulator-for-CatGenie-120/raw/master/catgenie.gif"/>

# Arduino Based CR14 Emulator for CatGenie 120
Suppress the DRM that prevents soap carterages from being refilled using an Arduino.

## Installation
This project can easily be built / flashed to an Arduino using [platform.io](https://platformio.org/) via: `pio run`. By default its setup to be used with an Arduino Uno but you can modify the platform.ini to support other Arduino variants.

After the code has been uploaded to the Arduino its ready to replace the CR14 RFID reader in the CatGenie 120. Disassemble the CatGenie and locate the card located next to the cartridge holder.
<img align="right" src="https://github.com/davidhampgonsalves/CR14-emulator-for-CatGenie-120/raw/master/cr14.jpg"/>

Unplug the CR14 and attach the I2C wires(white/black on my machine) to pins A4/A5 and 5v power(red/blue).

Remember to keep the CR14 for later projects,

## Alternatives
If you want a less DIY option check out the [CartridgeGenius](http://cartridgegenius.com/) which I've read is a compitent solution.
