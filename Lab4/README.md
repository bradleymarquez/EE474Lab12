Lab 4: Interrupts and Realtime
EE 474 Spring 2016
Workstation #12
Contributors: Brad Marquez, Joseph Rothlin, Aigerim Shintemirova

## Synopsys

The purpose of this lab is to further develop our embedded Linux skills by
starting to get the RoboTank system moving. Unlike the last lab, this lab was
implemented purely implemented in userspace. We implemented infrared sensors on
each side of the RoboTank (4 total) to sense and alert the tank when an obstacle
was close to the tank. If an obstacle was sensed, our program wrote to a named
pipe telling which side an obstacle was sensed, and an interrupt was sent to the
master program that controlled the H-bridge. Our master program, the H-bridge
driver automatically starts off by driving forward. It then changes directions
whenever its sensors sense an obstacle

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
	
## Notes - hBridgeTest.c
This file acted as the master program and driver for the H-bridge, and thus both
motors. A Serial-to-parallel shift register is used to control the H-bridge in
order to conserve precious GPIO pins. The parallel register on the shift register
correspond to the H-bridge inputs as follows: (QA: Standby, QB: AIN1, QC: AIN2,
QD: BIN1, QE, BIN2, QF: N/A, QG: N/A, QE: N/A). Note that A corresponds to the
left motor and B corresponds to the right motor. Initially, the H-bridge is set
to drive forward, and then responds to the environment depending on intterupts
sents by the attached sensors.

## Notes - sensorDriver.c
This file acted as the driver for the sensors. This program takes an ADC sample
every 25 milliseconds, and takes an average of the ADC samples every 50 samples,
using a timer interrupt. The slave program then sends an interrupt to the master
H-Bridge program if the calculated average is less than a threshold voltage that
indicates that the sensors are detecting a nearby obstacle. This program writes
what side has a sensor that is detecting an obstacle to a FIFO, which another
program can then read and interpret.
