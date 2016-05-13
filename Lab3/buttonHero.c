/* buttonHero.c
 * Brad Marquez, Joseph Rothlin, Aigerim Shintemirova
 * 11 / May / 2016
 *
 * TO DO:
 *  - clean code
 *  - comment code
 *  - create constants for integers used
 *  - lab report
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <time.h>
#include <math.h>
//#include <ctype.h>
//#include <signal.h>
//#include <fcntl.h>
//#include <termios.h>

#define SCREEN_SIZE 16 // total length of a line on the LCD screen
#define WRONG_GUESSES 8 // has to be less than or equal to 8
#define DELAY_TIME 1000 // time between each input update
#define NEW_LCD_DIR "/dev/lcd_driver" // lcd driver directory
#define NEW_BUT_DIR "/dev/button_driver" // button driver directory

#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3
#define PRESS 4
#define NUM_BUTTONS 5

// period of notes used in nanoseconds
const int noteA = 2272727;
const int noteB = 2024783;
const int noteC = 1912046;
const int noteD = 1702620;
const int noteE = 1517451;

static int fd_lcd, fd_but;
static FILE *sys2, *dirduty, *dirT;

void pressAnyButton();
int wantToQuit();
int mygetch(void);
void sigHandler(int);
int printLose(int, int);
void openPath(void);
void instructions(void);
void playGame(void);
void buzzer(FILE*, FILE*, int, int);
void closeBuzzer(void);
int main() {
	signal(SIGINT, sigHandler);
	openPath();
	instructions();
	playGame();
	close(fd_lcd);
	close(fd_but);
	return 0;
}

// Plays the game Button Hero on the attached LCDs
void playGame(){
	srand(time(NULL));
	int highScore = 0;
	char cont[100];
	cont[0] = ' ';
	int quit = 0;
	int inputs[NUM_BUTTONS] = {0, 0, 0, 0, 0};
	
	// main game loop
	while (!quit) {
		// Sets up piezobuzzer for sound
		sys2 = fopen("/sys/devices/bone_capemgr.9/slots", "w");
		fseek(sys2, 0, SEEK_END);
		fprintf(sys2, "am33xx_pwm");
		fflush(sys2);
		fprintf(sys2, "bone_pwm_P9_14");
		fflush(sys2);
		// Sets the pointers to the appropriate duty and period files
		dirduty = fopen("/sys/devices/ocp.3/pwm_test_P9_14.15/duty", "w");
		dirT = fopen("/sys/devices/ocp.3/pwm_test_P9_14.15/period", "w");
		
		int misses = -1;
		int currentScore = 0;
		int counter = 0;
		int rightInput = 0;
		
		// Splash screen before game start
		char *playScreen = "                Press button    to start!       ";
		write(fd_lcd, playScreen, SCREEN_SIZE * 3);
		pressAnyButton();
		usleep(500000);

		// initializes playing screen
		char screen[SCREEN_SIZE + 1];
		int i;
		for (i = 0; i < SCREEN_SIZE; i++) {
			screen[i] = ' ';
		}
		screen[SCREEN_SIZE] = '\0';
		
		// game starts
		int noteType = 5;
		int inputted, index;
		while (misses < WRONG_GUESSES){
			
			// checks if user gets a hit or miss
			if (counter == 0) {
				if (rightInput && screen[0] != ' ') { // if right input
					currentScore++;
				} else if (!rightInput) { // if wrong input
					misses++;
				}
				
				rightInput = 0;
				inputted = 0;
				noteType = rand() % 6;
			
				// shifts array to the right
				for (i = 0; i < SCREEN_SIZE - 1; i++) {
					screen[i] = screen[i + 1];
				} 

				// adds random note to game screen
				if (noteType == 0) {
					screen[SCREEN_SIZE - 1] =  '^'; // up arrow 
				} else if (noteType == 1) {
					screen[SCREEN_SIZE - 1] =  'v'; // down arrow 
				} else if (noteType == 2) {
					screen[SCREEN_SIZE - 1] =  '<'; // left arrow 
				} else if (noteType == 3) {
					screen[SCREEN_SIZE - 1] =  '>'; // right arrow 
				} else if (noteType == 4) {
					screen[SCREEN_SIZE - 1] =  'o'; // press button
				} else {
					screen[SCREEN_SIZE - 1] =  ' '; // space = no input
				}
			
				// builds string for current score
				char scoreString[17];
				sprintf(scoreString, "Score: %d", currentScore);
				for (i = strlen(scoreString); i < SCREEN_SIZE; i++) {
					scoreString[i] = ' ';
				}
				scoreString[SCREEN_SIZE] = '\0';
				
				// builds string for current misses
				char missMarks[17];
				strcpy(missMarks, "Misses: "); // 8 characters		
				for (i = SCREEN_SIZE - WRONG_GUESSES; i < (SCREEN_SIZE - WRONG_GUESSES + misses); i++) {
					missMarks[i] = 'X';
				}
				for (i = (SCREEN_SIZE - WRONG_GUESSES + misses); i < SCREEN_SIZE; i++) {
					missMarks[i] = ' ';
				}
				missMarks[SCREEN_SIZE] = '\0'; // needed if we concatenate anyway???
				
				// builds total string that is passed to the LCD driver
				char total[SCREEN_SIZE * 3];
				strcpy(total, screen); // first 16 chars: playing screen
				strcat(total, scoreString); // next 16 char: current score
				strcat(total, missMarks); // last 16 chars: misses
				
				// prints onto appropriate LCD screens (32 in first, 16 on second)
				write(fd_lcd, total, SCREEN_SIZE * 3);
				
				// prints screen, score, and misses to terminal
				for (i = 0; i < strlen(total); i++) {
					if (i % 16 == 0) {
						printf("\n");
					}
					printf("%c",total[i]);
				}
				printf("\n");			
			}
				// delay in between input update
				usleep(DELAY_TIME);
				
				// takes in inputs from user
				read(fd_but, inputs, NUM_BUTTONS * sizeof(int));
				index = 5;
				for (i = 0; i < NUM_BUTTONS; i++) {
					if (inputs[i] == 1) {
						index = i;
					}
				}
				
				// processes input, plays corresponding sound on buzzer
				char note;
				if (index == 0) {
					note =  '^'; // up arrow
					buzzer(dirduty, dirT, noteA, counter);
				} else if (index == 1) {
					note =  'v'; // down arrow 
					buzzer(dirduty, dirT, noteB, counter);
				} else if (index == 2) {
					note =  '<'; // left arrow 
					buzzer(dirduty, dirT, noteC, counter);
				} else if (index == 3) {
					note =  '>'; // right arrow 
					buzzer(dirduty, dirT, noteD, counter);
				} else if (index == 4) {
					note = 'o'; // press button
					buzzer(dirduty, dirT, noteE, counter);
				} else {
					note =  ' '; // space = no input
					buzzer(dirduty, dirT, 0, counter);
				}

				if (note == screen[0] && index != 5 && !inputted) { // if the input matches the note
					rightInput = 1;
					inputted = 1;
				} else if ((index != 5 && note != screen[0]) || (screen[0] == ' ' && index != 5)) { // if there is an input and the input is wrong
					rightInput = 0;
					inputted = 1;
				} else if (screen[0] == ' ' && index == 5 && !inputted) { // if there is a space note and nothing is pressed
					rightInput = 1;
				}
				
				// sets game screen update speed
				if (currentScore < 250) {
					counter = (counter + 1) % (750 - currentScore * 3); // increasing difficulty linearly with score
				} else {
					counter = (counter + 1) % 2; // cap on difficulty increase
				}
		}
		
		// closses buzzer files
		closeBuzzer();
		
		// prints game over scree 
		highScore = printLose(currentScore, highScore);
		usleep(1000000);
		
		// prompts user to play again
		pressAnyButton();
		usleep(500000);
		quit = wantToQuit();
		usleep(500000);
	}
	
	// closes LCD/button dev files
	close(fd_lcd);
	close(fd_but);
}

// Closes files associated with the buzzer
void closeBuzzer() {
	buzzer(dirduty, dirT, 0, 0);
	fclose(sys2);
	fclose(dirduty);
	fclose(dirT);
}

// Plays given sound on the buzzer
void buzzer(FILE *dirduty, FILE *dirT, int note, int counter) {
	if (counter % 15 == 0) {
		fprintf(dirT, "%d", note);
		fflush(dirT);

		fprintf(dirduty, "%d", note / 2);
		fflush(dirduty);
	}
}

// Sets up the path to the FIFO in order to interface with the LCD
void openPath(){
	fd_lcd = open(NEW_LCD_DIR, O_RDWR);
	fd_but = open(NEW_BUT_DIR, O_RDWR);
	if (fd_lcd < -1 || fd_but < -1) {
		if (fd_lcd < -1) {
			printf("File %s cannot be opened\n", NEW_LCD_DIR);
			exit(1);
		} else {
			printf("File %s cannot be opened\n", NEW_BUT_DIR);
			exit(1);
		}
	}
}

// Prints the losing screens on the attached LCDs
int printLose(int currentScore, int highScore) {
	int newHighScore;
	char winScreen[SCREEN_SIZE * 3];
	int i;
	if (currentScore > highScore) {
		newHighScore = currentScore;
		strcpy(winScreen, "NEW HIGH SCORE! ");
	} else {
		newHighScore = highScore;
		strcpy(winScreen, "SORRY, YOU LOST!");
	}
	
	// builds string for current score
	char scoreString[17];
	sprintf(scoreString, "Score: %d", currentScore);
	for (i = strlen(scoreString); i < SCREEN_SIZE; i++) {
		scoreString[i] = ' ';
	}
	scoreString[SCREEN_SIZE] = '\0';
	strcat(winScreen, scoreString);
	
	// builds string for current high score
	char highString[17];
	sprintf(highString, "High Score: %d", highScore);
	for (i = strlen(highString); i < SCREEN_SIZE; i++) {
		highString[i] = ' ';
	}
	highString[SCREEN_SIZE] = '\0';
	strcat(winScreen, highString);
	write(fd_lcd, winScreen, SCREEN_SIZE * 3);
	return newHighScore;
}

// Sets the LCD/button to its off state if Ctrl+C (signal interrupt) is passed by the user
void sigHandler(int signo) {
	if (signo == SIGINT) {
		closeBuzzer();
		close(fd_lcd);
		close(fd_but);
		exit(0);
	}
}

// Prints the instructions for the user to view on the terminal on game start up
void instructions(){
	printf("\nHello! Welcome to Button Hero!\n\nINSTRUCTIONS: Playing this game requires one user. Press the corresponding\n");
	printf("button when it gets to the far left of the single lined screen. Current score\nand the number of misses are displayed ");
	printf("on the two-lined LCD screen. A miss is\ngiven on a wrong input or when the user misses an input. The user is allowed %d\nmisses", WRONG_GUESSES);
	printf(" until they lose. The current high score is then displayed to the user\nand the user is prompted to play again.\n");

	printf("\nTypes of Notes:\n");
	printf("1. ^ = up on the joystick\n");
	printf("2. v = down on the joystick\n");
	printf("3. < = left on the joystick\n");
	printf("4. > = right on the joystick\n");
	printf("5. o = press the joystick\n");
	printf("6.   = do nothing\n");	
}

// Waits for the user to input anything on the button
void pressAnyButton() {
	int input[NUM_BUTTONS] = {0, 0, 0, 0, 0};
	while (!(input[UP] || input[DOWN] || input[LEFT] || input[RIGHT] || input[PRESS])) {
		read(fd_but, input, NUM_BUTTONS * sizeof(int));
	}
}

// Asks user if they would like to play again
int wantToQuit() {
	char quitScreen[SCREEN_SIZE * 3] = "Play again?      >No   Yes                      ";
	write(fd_lcd, quitScreen, SCREEN_SIZE * 3);
	int input[NUM_BUTTONS] = {0, 0, 0, 0, 0};	
	int cursorOnNo = 1;
	while (input[4] != 1) {
		read(fd_but, input, (NUM_BUTTONS * sizeof(int)));
		if ((cursorOnNo == 1) && (input[RIGHT] == 1)) {
			cursorOnNo = 0;
			quitScreen[17] = ' ';
			quitScreen[22] = '>';
			write(fd_lcd, quitScreen, SCREEN_SIZE * 3);
		} else if ((cursorOnNo == 0) && (input[LEFT] == 1)) {
			cursorOnNo = 1;
			quitScreen[17] = '>';
			quitScreen[22] = ' ';
			write(fd_lcd, quitScreen, SCREEN_SIZE * 3);
		}
	}
	return cursorOnNo;
}


	

