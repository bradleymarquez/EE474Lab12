Lab 1: Meeting the Beaglebone Black and the Linux
Operating System
EE 474 Spring 2016
Workstation #12
Contributors: Brad Marquez, Joseph Rothlin, Aigerim Shintemirova

## Synopsys

The purpose of this lab is to introduce the Beaglebone and the Linux OS.
In the first part of the lab we wrote a C program to display an 8-bit counter
between 3 LEDs on the Beaglebone. In part 2, we used PWM pins on the Beaglebone
to play an analog sound. The code was added to the original code from part 1. 
As a result, the PWM played a different sound for each count value.

## Code Execution
1. SSH protocol is used to configure the GPIO and PWM pins.
2. GPIO pins 45, 47, and 27 are used.
3. In order to transfer the file to the board use command:
	scp music.c root@192.168.7.2:~
4. To access the pins use commands:
	echo PIN_NUMBER > /sys/class/gpio/export
	
## Notes - Music.c
First, pointers are initialized and the paths to the gpio and pwm file systems are indicated in order to access and write to the specific files.
Then, we specified the exact GPIO and PWM pins using fprintf() and fflush() in order to stream it back to the file systems.
Directions for all the GPIOs to output are initialized next as well as pathways to the duty cycle and period files for PWM.

Next, we created a counter in order to display values 0...7 on 3 LEDs on the Beaglebone.
Also, we assigned a unique sound/note to each value of the counter based on the lab's specs.
The frequencies were picked to be to emulate the Imperial March sound:
		const int noteAb = 2409639;
		const int noteA = 2272727;
		const int noteF = 2865330;
		const int noteC = 1912046;
		const int noteE = 1517451;
		const int noteF2 = 1432665;
		const int noteA2 = 1136364;
		const int noteAb2 = 1203934;
		const int noteG2 = 1351370;
		const int noteGb2 = 1275526;
		const int noteBb = 2145186;
		const int noteEb = 1607071;
		const int noteD = 1702620;
		const int noteDb = 1803849;
		const int noteB = 2024783;







