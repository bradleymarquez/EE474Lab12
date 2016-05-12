/* buttonHero.c
 * Brad Marquez, Joseph Rothlin, Aigerim Shintemirova
 * 11 / May / 2016
 *
 *	
 *
 *	
 *	
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <time.h>
#include <math.h>

#define SCREEN_SIZE 16
#define WRONG_GUESSES 8 // has to be less than or equal to 8
#define DELAY_TIME 1000 // 1000 us
#define NEW_CHAR_DIR "/dev/lcd_driver"

static int fd;
static char *write_buf;

int mygetch(void);
void sigHandler(int);
int main() {
	srand(time(NULL));
	// Sets up the path to the FIFO in order to interface with the LCD
	fd = open(NEW_CHAR_DIR, O_RDWR);
	signal(SIGINT, sigHandler);
	if (fd < -1) {
		printf("File %s cannot be opened\n", NEW_CHAR_DIR);
		exit(1);
	}
	
	// Prints the instructions for the user to view on the terminal
	printf("\nHello! Welcome to Button Hero!\n\nINSTRUCTIONS: Playing this game requires one user. Press the corresponding\n");
	printf("button when it gets to the bottom of the single lined screen. Current score and\nnumber of misses are displayed");
	printf("on the two-lined LCD screen. A miss is either a wrong or a missed input.\nThe user is allowed %d misses", WRONG_GUESSES);
	printf("until they lose.\nThe current high score is then displayed to the user and the user is prompted to play again.\n");
	
	int highScore = 0;
	char cont = ' ';
	// reset while loop
	while (cont != 'q' || cont != 'Q') {
		int misses = 0;
		int currentScore = 0;
		int counter = 0;
		int rightInput = 1;
		
		printf("\n\nPress any key to continue.\n");
		mygetch(); // waits for any user input
		
		char screen[SCREEN_SIZE];
		int i;
		for (int i = 0; i < SCREEN_SIZE; i++) {
			screen[i] = ' ';
		}
		int noteType;
		int inputted;
		// playing while loop
		while (misses < WRONG_GUESSES){
			// checks if user missed a note
			if (counter == 0) {
				if (rightInput) {
					currentScore++;
				} else {
					misses++;
				}
				rightInput = 0;
				noteType = rand() % 4;
			
				// shifts array to the right
				for (i = SCREEN_SIZE; i > 0; i--) {
					screen[i] = screen[i - 1];
				} 

				// build string for screen
				if (noteType == 0) {
					strcat(screen, ">"); // right arrow ">"
				} else if (noteType == 1) {
					strcat(screen, "<"); // left arrow "<"
				} else if (noteType == 2) {
					strcat(screen, "v"); // down arrow "v"
				} else {
					strcat(screen, "^"); // up arrow "^"
				}
			
			
				// build string for current score
				char scoreString[17];
				sprintf(scoreString, "Score: %d", currentScore);
				for (i = strlen(scoreString); i < SCREEN_SIZE; i++) {
					scoreString[i] = ' ';
				}
				scoreString[SCREEN_SIZE] = '\0'; // needed?
				
				
				
				
				
				// build string for current misses
				char missMarks[17];
				strcpy(missMarks, "Misses: "); // 8 characters
				for (i = SCREEN_SIZE - WRONG_GUESSES; i < SCREEN_SIZE; i++) {
					missMarks[i] = 'X';
				}
				missMarks[SCREEN_SIZE] = '\0'; // needed if we concatenate anyway???
			
			
				char total[SCREEN_SIZE * 3];
				strcpy(total, screen); // first 16: playing screen
				strcat(total, scoreString); // next 16 char: current score
				strcat(total, missMarks); // last 16 chars: misses
			
			
				// print onto appropriate LCD screens (32 in first, 16 on second)
				write(fd, total, SCREEN_SIZE * 3);

				
			}
			// take input from user
			usleep(DELAY_TIME);

				// update inputted

				// if right timing && right input, +1 point, else +1 miss
				// input = button press from GPIO
				/*if (input == screen[0] && !inputted) {
					rightInput = 1;
				}
				*/
				counter = (counter + 1) % 250; // 1/4 second
		}
		
		// display lose screen
		if (currentScore > highScore){
			highScore = currentScore;
			printf("YOU GOT A HIGH SCORE OF %d!!!\n", highScore); // on LCD instead???
		}
		
		printf("\nPress 'q' to quit, or any other key to continue playing.\n");
		cont = getchar();
		getchar(); // gets rid of hanging \n
	}
	close(fd);
	free(write_buf);
	return 0;
}

// Waits for any user input, lets the program continue once an input is passed
int mygetch(void) {
  int ch;
  struct termios oldt, newt;

  tcgetattr ( STDIN_FILENO, &oldt );
  newt = oldt;
  newt.c_lflag &= ~( ICANON | ECHO );
  tcsetattr ( STDIN_FILENO, TCSANOW, &newt );
  ch = getchar();
  tcsetattr ( STDIN_FILENO, TCSANOW, &oldt );

  return ch;
}

// Sets the LCD to its off state if Ctrl+C (signal interrupt) is passed by the user
void sigHandler(int signo) {
	if (signo == SIGINT) {
		free(write_buf);
		close(fd);
		exit(0);
	}
}



	
