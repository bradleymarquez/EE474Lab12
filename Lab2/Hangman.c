/*	Hangman.c
 * Brad Marquez, Joseph Rothlin, Aigerim Shintemirova
 * 22 / April / 2016
 *
 *	Hangman game implemented on C. Two users interface with the terminal and the current game state is shown on the LCD.
 * 	Plays one game of hangman. User One inputs a word and User Two will guess characters until they guess too many wrong
 * 	characters or they guess all of the characters right. 
 *	
 *	
 */

#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include "ledinit.h"

#define MAX_STRING_LEN 16
#define WRONG_GUESSES 6

int mygetch(void);
void printMan(int);
void sigHandler(int);
void writeToPipe(char[], int, int);
int main() {
	int fd = lcdBoot();
	printf("after boot%d\n", fd);
	signal(SIGINT, sigHandler);
	if (fd == -1) {
		printf("Error on lcd boot: %s\n", strerror(errno));
	}
	printf("\nHello! Welcome to Hangman!\n\nINSTRUCTIONS: Playing this game requires two users. First, User One will be prompted to");
	printf(" enter\na word from 1-%d characters. The word inputted may only include characters [a-z, A-Z] and\nnumbers [0-9]. The word", MAX_STRING_LEN);
	printf(" will terminate on whitespace, and the word used will be the string\nbefore any whitespace input. ");
	printf("This word will then be displayed on the top line of the LCD,\nrepresented by \"_\" characters. ");
	printf("User Two will then be expected to input guesses in an attempt\nto guess the word that User One inputted. User Two may input multiple characters but");
	printf(" the guess\nused will be the first character inputted. User Two is alloted %d wrong character", WRONG_GUESSES);
	printf(" guesses until\nthey lose the game. Wrong character guesses made will be displayed on the bottom line of the LCD.");
	printf("\n\nPress any key to continue.\n");
		mygetch(); // waits for any user input
		char word[MAX_STRING_LEN];
		int ask = 1;
		while (ask) { // gets input from User One for word to use
			printf("\nUSER ONE: Please input your word (Max. %d characters).\n", MAX_STRING_LEN);
			char temp[16];
			scanf("%16[0-9a-zA-Z]", temp);
			int ch;
			while ((ch=getchar()) != EOF && ch != '\n');
			int i;
			for(i = 0; i < strlen(temp); i = i + 1){
				temp[i] = tolower(temp[i]);
			}
			if (strlen(temp) > MAX_STRING_LEN || strlen(temp) == 0) {
				printf("The word has an illegal amount of characters. Please try again.\n");
			} else {
				memmove(word, temp, strlen(temp) + 1);
				ask = 0;
				printf("%s", word);
			}
		}
		int i;
		for (i = 0; i < 100; i = i + 1) { // Shift terminal down 100 lines to hide the chosen word from User Two
			printf("\n");
		}
		// Print strlen(word) "_" characters on LCD first line
		char current[MAX_STRING_LEN];
		memmove(current, word, strlen(word) + 1);
		for (i = 0; i < strlen(word); i = i + 1) { // initializes word display line with padded spaces to 16 characters long
			current[i] = '_';
		}
		for (i = strlen(word); i < MAX_STRING_LEN - 1; i = i + 1) {
			current[i] = ' ';
		}
		current[MAX_STRING_LEN] = '\0';
		char wrongGuesses[MAX_STRING_LEN];
		int wrong = 0;
		int win = 0;
		for (i = 0; i < MAX_STRING_LEN; i = i + 1) { // Initializes wrongGuesses to be blank and 16 characters long
			wrongGuesses[i] = ' ';
		}
		while (wrong < WRONG_GUESSES) {
			printf("Word: %s\n", current); // Print current to top line of LCD
			writeToPipe(current, fd, TOP); // ********************************
			printf("Wrong Guesses: %s\n", wrongGuesses); // Print wrongGuesses to bottom line of LCD
			writeToPipe(wrongGuesses, fd, BOTTOM); // *****************
			if(wrong == WRONG_GUESSES - 1) {
				printf("\nWARNING: One more wrong guess will result in a loss.\n\n");
			}
			printf("USER TWO: Please input a character guess.\n");
			char inputChar;
			scanf("\n%c", &inputChar);
			int ch;
			while ((ch=getchar()) != EOF && ch != '\n');
			inputChar = tolower(inputChar);
			int j;
			int guessed = 0;
			for (j = 0; j < strlen(wrongGuesses); j = j + 1) {
				if (wrongGuesses[j] == inputChar) {
					guessed = 1;
				}
			}
			for (j = 0; j < strlen(current); j = j + 1){
				if (current[j] == inputChar) {
					guessed = 1;
				}
			}
			int found = 0;
			for (j = 0; j < strlen(word); j = j + 1) {
				if (word[j] == inputChar) {
					current[j] = inputChar;
					found = 1;
				}
			}
			if (!found && !guessed) {
				printf("\nLetter not found!\n");
				wrongGuesses[wrong * 2] = inputChar;
				wrongGuesses[wrong * 2 + 1] = ' ';
				wrong = wrong + 1;
			} else if (guessed) {
				printf("\nYou have already guessed this character.\n");
			} else {
				printf("\nLetter found!\n");
			}
			int foundAll = 1;
			for (j = 0; j < strlen(current); j = j + 1) {
				if (current[j] == '_') {
					foundAll = 0;
				}
			}
			if (foundAll) {
				win = 1;
			}
			if (win) {
				wrong = WRONG_GUESSES + 1;
			}
			printf("\n");
			printMan(wrong);
		}
		if (win){
			printf("%s\nYOU WIN!!!\n", current);
			// display win message on LCD top line
			writeToPipe("CONGRATULATIONS!", fd, TOP);
			writeToPipe("YOU WIN!        ", fd, BOTTOM);
		} else {
			printf("%s\nYOU LOSE!!!\n", current);
			//display lose message on LCD top line
			writeToPipe("SORRY :(        ", fd, TOP);
			writeToPipe("YOU LOSE!       ", fd, BOTTOM);
		}
	printf("\nPress any key to exit.\n");
	mygetch(); // waits for any user input
	closeLCD();
	return 0;
}

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

void writeToPipe(char send[], int fd, int line) {
	  ssize_t bytes, written = 0;
	  while (written < SCREEN_SIZE) {
	    bytes = write(fd, send, SCREEN_SIZE - written);
	    if (bytes == -1) {
	      if (errno != EINTR) {
		printf("During write%d\n", fd);
		printf("Error on write: %s\n", strerror(errno));
		// clear the pipe
		return;
	      }
	      continue;
	    }
	    written += bytes;
	  } 
	if (printScreen(fd, line) == -1) {
		printf("Error on print: %s\n", strerror(errno));
	}
}

void printMan(int i) {
	 switch (i) {
	      case 0 :
	      printf("  _______\n");
	      printf("  |/\n");
	      printf("  |\n");
	      printf("  |\n");
	      printf("  |\n");
	      printf("  |\n");
	      printf("__|_________\n\n");
	     break;
	     case 1:
	      printf("  _______\n");
	      printf("  |/   | \n");
	      printf("  |    O \n");
	      printf("  |\n");
	      printf("  |\n");
	      printf("  |\n");
	      printf("__|_________\n\n");
	     break;
	     case 2:
	      printf("  _______\n");
	      printf("  |/   | \n");
	      printf("  |    O \n");
	      printf("  |    |\n");
	      printf("  |    |\n");
	      printf("  |\n");
	      printf("__|_________\n\n");
	     break;
	     case 3:
	      printf("  _______\n");
	      printf("  |/   | \n");
	      printf("  |    O \n");
	      printf("  |   \\|\n");
	      printf("  |    | \n");
	      printf("  |\n");
	      printf("__|_________\n\n");
	     break;
	     case 4:
	      printf("  _______\n");
	      printf("  |/   | \n");
	      printf("  |    O \n");
	      printf("  |   \\|/\n");
	      printf("  |    | \n");
	      printf("  |\n");
	      printf("__|_________\n\n");
	     break;
	     case 5:
	      printf("  _______\n");
	      printf("  |/   | \n");
	      printf("  |    O \n");
	      printf("  |   \\|/\n");
	      printf("  |    | \n");
	      printf("  |   /\n");
	      printf("__|_________\n\n");
	     break;
	     case 6:
	      printf("  _______\n");
	      printf("  |/   | \n");
	      printf("  |    X \n");
	      printf("  |   \\|/\n");
	      printf("  |    | \n");
	      printf("  |   / \\\n");
	      printf("__|_________\n\n");
	     break;
	 }
}

// Sets the LCD to its off state if Ctrl+C (signal interrupt) is passed by the user
void sigHandler(int signo) {
	if (signo == SIGINT) {
		closeLCD();
		exit(0);
	}
}
	
