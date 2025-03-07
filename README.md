Zed Switch with the following functionality:

Fail-safe and non-fail-switching for up to four circuits. Both use n-channel mosfets. I tried to use a p-channel high side switch but the the gpio pins could not reliably pull the mosfet gate high (and so turn the circuit off.)

Instead, I am pulling the mosefet gate high using a 4.6 Kohm resitor to the 12V (+) rail and pulling low with an NPN transitor (HiLetgo 2N3904) with the emitter driving the gate low. (the collector is connected to the 12V and the base to the a GPIO.)

This should work with low ad for high-current switching such as pumps.

I have used several kinds of buck step-downs. The 12v rail feeds the step-down, and the step-down feeds the 5v rail. 

The UART comms work on the default rx tx pins and send power status info to a connected device as a redundancy to wireless comms. This is primarily built for use with a supercapacitor but offers some useful info on the switch power supply. The model SC  (super capacitor) is meant to allow devices to clean up any data before losing power.

Power is monitored via an I2C INA219 module. Ina260 could be switched out with minor code changes. Given there is only one INA device, the INA is initialized separately from other switch circuits. I did intend to have a monitor for each circuit but for now just one.

USB can be connected 5V rail is energized. There is a shotsky diode between the MCU's 5v pin and 5v rail. BUT when doing so without the 12v/5v power supply the LED module will fail. 

The LED module drived the LED usings the PCF8574 IO Expansion Board. 

Hardware:

Xiao ESP32 c6 or s3 x1
IRLZ44N N-channel mosfet x4  (low-side switch)
NPN transitor (HiLetgo 2N3904) x4
12v to 5v buck stepdown x1
SB140-E3/54 schotsky diode x1
PCF8574 PCF8574 IO Expansion Board X1
Various LEDs x5

