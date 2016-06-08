Lab 5: Final Project
EE 474 Spring 2016
Workstation #12
Contributors: Brad Marquez, Joseph Rothlin, Aigerim Shintemirova

## Synopsys

The goal of this project was to utilize the tools and skills we learned in the 
previous labs, e.g. GPIO pins functionality. motors and sensors control, Bluetooth.
We were able to achieve control of the motors through a one-way bluetooth transmission
from a Windows Surface to the BlueSMiRF. This program is ran through on bootup through
a service that we set up.

We also designed a system that scans the space around it with 4 sensors mounted on a
servo. We are then able to send this data back to a terminal emulator, where the data
can be picked up and read to map out the Robo-Tank’s surroundings.

Basic Functionality:
	Drive Program
		Q- Forward
		A- Backward
		W- Right
		S- Left
	
	Sensing Program
		W- Forward
		S- Backward
		A- Left
		D- Right
		E- Scan

## Code Execution
1. SSH protocol is used to operate the Beaglebone
2. A shift register was used to transfer data from the Beaglebone to the H-bridge
3. To compile the sensor and H-bridge driver files, use the command:
	make
3. In order to transfer the files to the board, use the command:
	make transfer
4. In order to operate the RoboTank without an attached ethernet or power cable,
	a Bash script is ran on startup.
5. For mobile power, the Beaglebone is powered by the attached power brick and
	the motors are powered by the attached battery pack (~6V).
	
## Notes - hBridgeDrive.c
This program establishes the connection with the Bluetooth and allows to send/receive
serial data. We used a NXT Bluetooth Controller from the Chrome Web Store, which is able
to send data bytes to the bluetooth. We then use these inputs to drive the RoboTank forward,
left, right, and backwards.

## Notes - hBridgeDriver.c
This file acted as the driver for the sensors. This program takes an ADC sample
every 25 milliseconds, and takes an average of the ADC samples every 50 samples,
using a timer interrupt. The slave program then sends an interrupt to the master
H-Bridge program if the calculated average is less than a threshold voltage that
indicates that the sensors are detecting a nearby obstacle. This program writes
what side has a sensor that is detecting an obstacle to a FIFO, which another
program can then read and interpret.

## Notes - ScannerControl.c
This program contains functions for a higher level program to easily control the
servo motor along with the distance sensors (this entire collection is referred to
as the scanner) in a concise encapsulated manner. This program allows for files
initialization, files closure, the ability to initiated a scan with various sample
rate settings, and functions which will print a set of scanner readings in both as
both a set of values and a visual text display (as seen in figure 5).

## Notes - servoControl.c
This program contains functions for a high level program to easily control the servo
motor within a concise encapsulation. A function is provided to initialize the PWM pin
files associated with the servo, set the servo to a specific angle, and close the PWM
pin file. 

## Notes - sensorControl.c
This program contains functions for a high level program to easily control the four
sensors attached to the servo providing concise encapsulation. A function is provided
to initialize the files associated with the sensors, read values from specific sensors,
and close all the files associated with the sensors.

## Notes - mapBuild.c
When the RoboTank is told to scan, after it is done, it sends back data through the
bluetooth. However, this data are non-displayable ASCII characters, so we made the
transmitted data have a range of A-Z so that it can be displayed on the terminal
emulator and printed to a txt file. This program can then parse the txt file, take
in the eligible data, and print out a map based on the data to the terminal.
