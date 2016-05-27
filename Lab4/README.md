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
	the Beaglebone is powered by the attached power brick, the motors are powered
	by the attached battery pack (~6V) and the 
	
## Notes - lcd\_driver.c
This file acted as the single driver for both LCDs. Using the system open/close
functions the user could intialize and uninitialize the LCD screens and GPIO
pins. Using the system write call the user could provide up to 48 characters
which would then be printed to the two screens. Using the seperate enable
pins for the LCDs this module could load the data to write to the shift
register and then choose which screen to write to by selecting which
enable pin to toggle.

## Notes - button\_driver.c
This file acted as the driver for the 5-way button. Using the system open/close
functions the user could initialize and uninitialize the GPIO pins associated
with the button. Using the system read call the user can receive 20 bytes
of data which contains the status of each direction on the button.

## Notes - buttonHero.c
This file was written in user space C and used both the modules above to
communicate with periferal devices while executing the game. This level
of abstraction made communicating with periferals fairly simple from
the perspective of buttonHero.c. This program creates the visual effect
of scrolling arrows across a screen during which the user is meant to
press the corresponding button direction assosiated with that arrow. High
scores are recorded and displayed at the end of each round, and a buzzer
along with LEDs were added (and controlled by buttonHero.c) in order to
increase the aesthetics of the game.