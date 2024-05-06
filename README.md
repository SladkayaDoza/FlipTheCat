# FlipTheCat

Multitool for working with radio signals, based on FlipperZero on the basis of esp32 with less functionality and completely self-assembly. Use cc1101 module.
___
I really want to write programs in flipper zero, but I don't have the money for it.  
I would appreciate any support.
Binance - 727946949

## Functions
- copying of an unlimited number of static signals (can be set in firmware)
- change signal code, rename signal name, delete code
- copy raw signal (start recording when signal appears on air), display signal data quantity, rename signal, delete signal
- On-air frequency analyzer, sensitivity setting, frequency tabular value output, received signal frequency output
- Brute force of 12 bit signals for opening of cinder barriers
- Bluetooth BLE emulation Keyboard (gamepad)
- GPIO Frequency generator, set frequency and filling
- Setting the broadcasting frequency and the start num of the 12-bit signal brute force
- Game 2048

## ToDo
- https://trello.com/b/FfUWApZh/flip-esp32

## Open Project
You can suggest changes or additional features

## Materials:
- cc1101 module
- lithium battery
- esp32 (s, s3, an other.., flash: 4+mb)
- 10 smd buttons (2 for up gamepad buttons)
- tp4057 type c charger with protection
- step up dc-dc and linear voltage stabilizer | or step up, down corverter 3.3v
- gpio output pins x18 (4+10+4)
- power switch
- 8 resistors 10k(5 for buttons, 2 for boot pins esp32, 1 for buzzer transistor), 3 resistors 100k (1 for buttons, 2 for volltage censor), 1 resistor 600 Ohm (for led)
- oled display 1.3"
- yellow led for indicator
- 2 capacitor (10v 220uf, 10v, 220pf)
- Buzzer 3.3v
- Transistor for buzzer

## After assembling
Once created, you need to calibrate the buttons and the resistive divider to output the battery voltage
* To calibrate the buttons, uncomment line 11 in the "tick.ino" file
* To calibrate the voltage you need to change the coefficients in the Function mainn()
According to the formula - getVolltage() * 3.26 * ((r1 + r2) / r2) / 4096
When connecting to a computer via UART, do not connect 3.3v from the programmer, for correct display of values from the buttons


##	Example of a finished product:
<img src="https://github.com/SladkayaDoza/FlipTheCat/blob/index.html/images/IMG_20240229_224614_977.jpg" width="500">
<img src="https://github.com/SladkayaDoza/FlipTheCat/blob/index.html/images/IMG_20240229_224615_312.jpg" width="500">
<img src="https://github.com/SladkayaDoza/FlipTheCat/blob/index.html/images/IMG_20240229_224504_879.jpg" width="500">
<img src="https://github.com/SladkayaDoza/FlipTheCat/blob/index.html/images/IMG_20240229_224555_843.jpg" width="500">