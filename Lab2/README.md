Lab 2: Inputs, Outputs, Time
EE 474 Spring 2016
Workstation #12
Contributors: Brad Marquez, Joseph Rothlin, Aigerim Shintemirova

## Synopsys

The purpose of this lab is to explore further the functionality of the
Beaglebone and experience the task working with a periferal device and creating
its' driver. In this lab we created a hangman game using a periferal LCD screen
as the disply. Our code consisted a driver program which was used to
manipulate the bits being sent and received from the LCD. Also, a top level
program which ran the game and used the driver program to interface
the LCD.

## Code Execution
1. SSH protocol is used to operate the Beaglebone
2. 14 Verious GPIO pins are used to connect to the 14 pins of the LCD
3. In order to transfer the file to the board use command:
	scp Hangman root@192.168.7.2:~
4. To access the pins we create a directory within the Beaglebon which contains
   files that store the values and properties of each pin.
	
## Notes - Ledinit.c
This file contains functions which are used by the client (Hangman.c) to interface with
the LCD. The user is able to clear, turn on/off, and print to the LCD with ease while
the Ledinit.c file takes care of the underlying signal manipulation. This file sets the
pins to the apprpopriate bits and sends the various signals necessary to communicate
to the LCD. 

## Notes - Hangman.c
This file runs the Hangman game and uses Ledinit.c to interface with the LCD. The
game asks the 1st player for a word that will be used as the guess. Then continously
prompts the 2nd player to guess letters until the game is either won or lost. All
while displying the current state of the game on both the console and LCD. 
