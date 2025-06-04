Zed Switch with the following functionality:

Fail-safe and non-fail-switching for up to four circuits. Both use n-channel MOSFETs. I tried to use a p-channel high-side switch, but the GPIO pins could not reliably pull the MOSFET gate high (and so turn the circuit off).

Instead, I am pulling the MOSFET gate high using a 4.6 kΩ resistor to the 12V (+) rail and pulling low with an NPN transistor (HiLetgo 2N3904), with the emitter driving the gate low. (The collector is connected to the 12V and the base to a GPIO.)

This should work with low and high-current switching, such as pumps.

I have used several kinds of buck step-downs. The 12v rail feeds the step-down, and the step-down feeds the 5v rail. 

The UART comms work on the default Rx and Tx pins and send power status information to a connected device as a redundancy to wireless comms. This is primarily built for use with a supercapacitor, but it offers some helpful information on the switch power supply. The model SC (supercapacitor) is meant to allow devices to clean up any data before losing power.

Power is monitored via an I2C INA260. Earlier versions used an INA 219 module. I switched out the module for just the chip, and the 260 is far more straightforward to wire (and more precise). Given that there is only one INA device, the INA is initialized separately from other switch circuits. I intended to have a monitor for each circuit, but I have just one for now.

USB can be connected when the 5V rail is energized. There is a Shotsky diode between the MCU's 5V pin and 5V rail. However, when doing so without the 12v/5v power supply, the LED module will fail. 

The LED module drives the LEDs using a PCF8574 IO Expansion Board. 

Hardware:

Xiao ESP32 c6 or s3 x1

IRLZ44N N-channel mosfet x4  (low-side switch)
NPN transitor (HiLetgo 2N3904) x4
12v to 5v buck stepdown x1
SB140-E3/54 schotsky diode x1
PCF8574 PCF8574 IO Expansion Board X1
Various LEDs x5
INA260 x1. (The logic/code is set up so the system could be adapted for one on each circuit.)

Version one design LED module
0=Orange
1=Green
2=Blue
3=White
