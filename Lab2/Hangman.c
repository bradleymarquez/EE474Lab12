/*	hangman.c
 * Brad Marquez, Joseph Rothlin, Aigerim Shintemirova
 * 22 / April / 2016
 *
 *	
 *  
 */

#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#define MAX_STRING_LEN 16
#define WRONG_GUESSES 6
int mygetch(void);
int main() {
	printf("\nHello! Welcome to Hangman!\nInstructions: Playing this game requires two users. First, User One will be prompted to\n");
	printf("enter a word from 1-%d characters. This word will then be displayed on the top line of the LCD,\nrepresented by \"_\" characters. ", MAX_STRING_LEN);
	printf("User Two will then be expected to input guesses in an attempt to guess\nthe word that User One inputted. User Two is alloted %d wrong character ", WRONG_GUESSES);
	printf("guesses until they\nlose the game. Wrong character guesses made will be displayed on the bottom line of the LCD.");
	printf("\n\nPress any key to continue.\n");
		mygetch();
		char word[MAX_STRING_LEN];
		int ask = 1;
		while (ask) {
			printf("\nUSER ONE: Please input your word (Max. %d characters).\n", MAX_STRING_LEN);
			char temp[10000];
			fflush(stdin);
			fflush(stdout);
			scanf("%16[0-9a-zA-Z]", temp);
			int i;
			for(int i = 0; i < strlen(temp); i = i + 1){
				temp[i] = tolower(temp[i]);
			}
			if (strlen(temp) > MAX_STRING_LEN) {
				printf("The word given was too long. Please try again.\n");
			} else {
				memmove(word, temp, strlen(temp));
				ask = 0;
			}
		}
		for (int i = 0; i < 100; i = i + 1) {
			printf("|||||||||||||||||||||||||||||||||||||||||||||||||||");
			printf("|||||||||||||||||||||||||||||||||||||||||||||||||||");
			printf("||||||||||||||||||\n");
		}
		printf("%s\n", word); // debug ***********
		// Print strlen(word) "_" characters on LCD first line
		char current[strlen(word)];
		for (int i = 0; i < strlen(word); i = i + 1) {
			current[i] = '_';
		}
		char wrongGuesses[12];
		int wrong = 0;
		int win = 0;
		while (wrong <= WRONG_GUESSES) {
			printf("USER TWO: Please input a character guess.\n");
			char inputChar;
			fflush(stdin);
			fflush(stdout);
			scanf(" %c\n", &inputChar);
			inputChar = tolower(inputChar);
			int j;
			int found = 0;
			for (int j = 0; j < strlen(word); j = j + 1) {
				if (word[j] == inputChar) {
					current[j] = inputChar;
					found = 1;
				}
			}
			if (!found) {
				wrong = wrong + 1;
				char strcat1[2] = {inputChar, '\0'};
				char strcat2[2] = {' ', '\0'};
				strcat(wrongGuesses, strcat1);
				strcat(wrongGuesses, strcat2);
			}
			// check if win
			int foundAll = 1;
			for (int j = 0; j < strlen(current); j = j + 1) {
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
			printf("%s", current);
		}
		if (win){
			printf("\n\nYOU WIN!!!");
			// display win
		} else {
			printf("\n\nYOU LOSE!!!");
			//display lose
		}
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
	