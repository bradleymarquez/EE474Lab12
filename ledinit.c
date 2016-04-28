/*	ledinit.c
 * Brad Marquez, Joseph Rothlin, Aigerim Shintemirova
 * 22 / April / 2016
 *
 *	
 *  
 */

#include <stdio.h>  // for File IO and printf
#include <unistd.h> // for usleep
#include <signal.h> // for user termination of the program

// Total number of pins used with LCD
#define TOTALPINS 11

// Pin location in direction/value array
#define RS 0
#define RW 1
#define E 2
#define DB0 3
#define DB1 4
#define DB2 5
#define DB3 6
#define DB4 7
#define DB5 8
#define DB6 9
#define DB7 10

#define LCD_4 48 // RS Pin - GPIO_PIN_48
#define LCD_5 49 // RW Pin - GPIO_PIN_49
#define LCD_6 60 // E Pin - GPIO_PIN_60
#define LCD_7 66 // DB0 Pin - GPIO_PIN_66
#define LCD_8 69 // DB1 Pin - GPIO_PIN_69
#define LCD_9 45 // DB2 Pin - GPIO_PIN_45
#define LCD_10 47 // DB3 Pin - GPIO_PIN_47
#define LCD_11 67 // DB4 Pin - GPIO_PIN_67
#define LCD_12 68 // DB5 Pin - GPIO_PIN_68
#define LCD_13 44 // DB6 Pin - GPIO_PIN_44
#define LCD_14 26 // DB7 Pin - GPIO_PIN_26
#define TEST 112 // Used for testing only

// Used to determine when Ctrl-C has been intiated
static volatile int keepRunning = 1;

void initialize(FILE *lcdPins[]);
void setAddress(unsigned char address, FILE* lcdPins[]);
void writeChar(unsigned char character, FILE* lcdPins[]);
void clearDisplay(FILE* value[]);
void displayOff(FILE* value[]);
void setBus(unsigned char byte, FILE* lcdPins[]);
void send(FILE *);
void closeLCD(FILE *lcdPins[]);
void sigHandler(int);

int main() {

	// Creates pointers to interface with the files of the Beaglebone
	FILE* direction[TOTALPINS];
	FILE* value[TOTALPINS];
	int gpioPins[TOTALPINS] = {LCD_4, LCD_5, LCD_6, LCD_7, LCD_8, LCD_9, LCD_10, LCD_11, LCD_12, LCD_13, LCD_14};
	FILE *sys, *dirTest, *test;

	// Opens sys file for creating GPIO directories
	sys = fopen("/sys/class/gpio/export", "w");
	fseek(sys, 0, SEEK_SET); 

	// Creates directories for all the GPIO pins used
	int i;
	for (i = 0; i < TOTALPINS; i++) {
		fprintf(sys, "%d", gpioPins[i]);
		fflush(sys);
	}
	fprintf(sys, "%d", TEST);
	fflush(sys);

	// Sets the direction of each GPIO to output
	direction[RS] = fopen("/sys/class/gpio/gpio48/direction", "w");
	direction[RW] = fopen("/sys/class/gpio/gpio49/direction", "w");
	direction[E] = fopen("/sys/class/gpio/gpio60/direction", "w");
	direction[DB0] = fopen("/sys/class/gpio/gpio66/direction", "w");
	direction[DB1] = fopen("/sys/class/gpio/gpio69/direction", "w");
	direction[DB2] = fopen("/sys/class/gpio/gpio45/direction", "w");	
	direction[DB3] = fopen("/sys/class/gpio/gpio47/direction", "w");
	direction[DB4] = fopen("/sys/class/gpio/gpio67/direction", "w");
	direction[DB5] = fopen("/sys/class/gpio/gpio68/direction", "w");
	direction[DB6]= fopen("/sys/class/gpio/gpio44/direction", "w");
	direction[DB7] = fopen("/sys/class/gpio/gpio26/direction", "w");
	for (i = 0; i < TOTALPINS; i++) {
		fseek(direction[i], 0, SEEK_SET);
		fprintf(direction[i], "%s", "out");
		fflush(direction[i]);
	}

	// Sets direction of GPIO pin used for testing
	dirTest = fopen("/sys/class/gpio/gpio112/direction", "w");
	fseek(dirTest, 0, SEEK_SET);
	fprintf(dirTest, "%s", "out");
	fflush(dirTest);

	// Opens the file that controls the pin value
	value[RS] = fopen("/sys/class/gpio/gpio48/value", "w");
	value[RW] = fopen("/sys/class/gpio/gpio49/value", "w");	
	value[E] = fopen("/sys/class/gpio/gpio60/value", "w");
	value[DB0] = fopen("/sys/class/gpio/gpio66/value", "w");	
	value[DB1] = fopen("/sys/class/gpio/gpio69/value", "w");
	value[DB2] = fopen("/sys/class/gpio/gpio45/value", "w");	
	value[DB3] = fopen("/sys/class/gpio/gpio47/value", "w");
	value[DB4] = fopen("/sys/class/gpio/gpio67/value", "w");	
	value[DB5] = fopen("/sys/class/gpio/gpio68/value", "w");
	value[DB6] = fopen("/sys/class/gpio/gpio44/value", "w");	
	value[DB7] = fopen("/sys/class/gpio/gpio26/value", "w");
	test = fopen("/sys/class/gpio/gpio112/value", "w");
	
	// Set test pin to zero
	fprintf(test, "%d", 0);
	fflush(test);

	// Initialize interupt function for Ctrl-C
	signal(SIGINT, sigHandler);

	initialize(value);

	setAddress((unsigned char) 0x00, value);

	writeChar((unsigned char) 'H', value);
	writeChar((unsigned char) 'e', value);
	writeChar((unsigned char) 'l', value);
	writeChar((unsigned char) 'l', value);
	writeChar((unsigned char) 'o', value);
	
	// Set test pin to 1 in order to confirm location arrived in code
	fprintf(test, "%d", 1);
	fflush(test);	
	
	while(keepRunning) {
	}
	
	closeLCD(value);

	// Closes all accessed files
	fclose(sys);
	for (i = 0; i < TOTALPINS; i++) {
		fclose(direction[i]);
		fclose(value[i]);
	}
	
	// returns 0 if program runs all the way through
	return 0;
}

// Sets the LCD to its off state if Ctrl+C (signal interrupt) is passed by the user
void sigHandler(int signo) {
	if (signo == SIGINT) {
		printf(" Caught SIGINT\n");
		keepRunning = 0;
	}
}

// Used to put the LCD screen into a shut down state
void closeLCD(FILE *value[]) {
	displayOff(value);
	clearDisplay(value);
}

void initialize(FILE* value[]) {
	usleep(15001);
	
	fprintf(value[RS], "%d", 0); // Function Set #1
	fflush(value[RS]);
	fprintf(value[RW], "%d", 0);
	fflush(value[RW]);
	setBus((unsigned char) 0x30, value);
	send(value[E]);
	fflush(value[E]);

	usleep(4101);
	
	send(value[E]); // Function Set #2
	fflush(value[E]);
	
	usleep(101);
	
	send(value[E]); // Function Set #3
	fflush(value[E]);
	
	usleep(500);

	setBus((unsigned char) 0x38, value); // Function Set #4
	send(value[E]);
	fflush(value[E]);
	
	usleep(500);

	displayOff(value);

	clearDisplay(value);

	setBus((unsigned char) 0x0c, value); // Entry Mode Set
	send(value[E]);
	fflush(value[E]);
	
	usleep(500);

	setBus((unsigned char) 0x0f, value); // Display on w/ cursor & blink on
	send(value[E]);
	fflush(value[E]);

	usleep(500);
}



// Flips enable pin which cause LCD to read current signal on the bus
void send(FILE *val6) {
	fprintf(val6, "%d", 1);
	fflush(val6);
	usleep(10);
	fprintf(val6, "%d", 0);
	fflush(val6);
	usleep(10);
}

// Turns the LCD off
void displayOff(FILE* value[]) {
	fprintf(value[RS], "%d", 0);
	fflush(value[RS]);
	fprintf(value[RW], "%d", 0);
	fflush(value[RW]);
	setBus((unsigned char) 0x08, value); // Display OFF
	send(value[E]);
	fflush(value[E]);
	usleep(50);
}

// Clears the LCD
void clearDisplay(FILE* value[]) {
	fprintf(value[RS], "%d", 0);
	fflush(value[RS]);
	fprintf(value[RW], "%d", 0);
	fflush(value[RW]);
	setBus((unsigned char) 0x01, value); // Clear Display
	send(value[E]);
	fflush(value[E]);
	usleep(50);
}

// Writes the "character" to the LCD screen ("character" expects the correct
// value based on the char table in the LCD spec)
void writeChar(unsigned char character, FILE* value[]) {
	fprintf(value[RS], "%d", 1);
	fflush(value[RS]);
	fprintf(value[RW], "%d", 0);
	fflush(value[RW]);
	setBus(character, value);
	send(value[E]);
	fflush(value[E]);
	usleep(50);
}

// Sets the R/W pointer to the address specified
void setAddress(unsigned char address, FILE* value[]) {
	fprintf(value[RS], "%d", 0); // Set DD RAM Address to 0
	fflush(value[RS]);
	fprintf(value[RW], "%d", 0);
	fflush(value[RW]);
	address |= 0x80;
	setBus(address, value);
	send(value[E]);
	fflush(value[E]);
	usleep(50);
}

// Sets DB7 to DB0 to the given 8 bits
void setBus(unsigned char byte, FILE* value[]) {
	fprintf(value[DB0], "%d", (byte % 2));
	fflush(value[DB0]);
	byte  = byte >> 1;
	fprintf(value[DB1], "%d", (byte % 2));
	fflush(value[DB1]);
	byte  = byte >> 1;
	fprintf(value[DB2], "%d", (byte % 2));
	fflush(value[DB2]);
	byte  = byte >> 1;
	fprintf(value[DB3], "%d", (byte % 2));
	fflush(value[DB3]);
	byte  = byte >> 1;
	fprintf(value[DB4], "%d", (byte % 2));
	fflush(value[DB4]);
	byte  = byte >> 1;
	fprintf(value[DB5], "%d", (byte % 2));
	fflush(value[DB5]);
	byte  = byte >> 1;
	fprintf(value[DB6], "%d", (byte % 2));
	fflush(value[DB6]);
	byte  = byte >> 1;
	fprintf(value[DB7], "%d", (byte % 2));
	fflush(value[DB7]);
}
