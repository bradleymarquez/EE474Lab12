/*	ledinit.c
 * Brad Marquez, Joseph Rothlin, Aigerim Shintemirova
 * 22 / April / 2016
 *
 * This program is used to interface with the Newhaven LCD.	
 *  
 */

#include "ledinit.h"

static FILE* value[TOTALPINS];
static char* path = "/root/fifo";
static void initialize();
static void setAddress(unsigned char address);
static void writeChar(unsigned char character);
static void setBus(unsigned char byte);
static void send();

// Sets up pointers to the Beaglebone files to interface with the LCD, initializes the LCD, and
// sets up the named pipe used to interface with the LCD
int lcdBoot() {
	// Creates pointers to interface with the files of the Beaglebone
	FILE* direction[TOTALPINS];
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

	// Initializes LCD
	initialize();
	
	// Creat/open pipe file
	if (access(path, F_OK) != 0) {
		printf("Creating pipe \"testfifo\"\n");
		mkfifo(path, 0666);
	}

	// Open pipe
	printf("Opening pipe\n");
	int fd = open(path, O_RDWR);
	printf("After open%d\n", fd);
	if (fd == -1) {
		printf("Error open: %s\n", strerror(errno));
		return -1;
	}

	// Closes all accessed files
	fclose(sys);
	for (i = 0; i < TOTALPINS; i++) {
		fclose(direction[i]);
	}
	return fd;
}

// Turns off and clears the LCD, closes accessed pointers, and deletes the named pipe used
void closeLCD() {
	displayOff();
	clearDisplay();
	int i;
	for (i = 0; i < TOTALPINS; i++) {
		fclose(value[i]);
	}
	unlink(path);
}

// Initializes the LCD by following a specific set of instructions
void initialize() {
	usleep(15001);
	
	fprintf(value[RS], "%d", 0); // Function Set #1
	fflush(value[RS]);
	fprintf(value[RW], "%d", 0);
	fflush(value[RW]);
	setBus((unsigned char) 0x30);
	send();
	fflush(value[E]);

	usleep(4101);
	
	send(); // Function Set #2
	fflush(value[E]);
	
	usleep(101);
	
	send(); // Function Set #3
	fflush(value[E]);
	
	usleep(500);

	setBus((unsigned char) 0x38); // Function Set #4
	send();
	fflush(value[E]);
	
	usleep(500);

	displayOff();

	clearDisplay();

	setBus((unsigned char) 0x0c); // Entry Mode Set
	send();
	fflush(value[E]);
	
	usleep(500);

	setBus((unsigned char) 0x0f); // Display on w/ cursor & blink on
	send();
	fflush(value[E]);

	usleep(500);
}

// Flips enable pin which cause LCD to read current signal on the bus
void send() {
	fprintf(value[E], "%d", 1);
	fflush(value[E]);
	usleep(10);
	fprintf(value[E], "%d", 0);
	fflush(value[E]);
	usleep(10);
}

// Turns the LCD off
void displayOff() {
	fprintf(value[RS], "%d", 0);
	fflush(value[RS]);
	fprintf(value[RW], "%d", 0);
	fflush(value[RW]);
	setBus((unsigned char) 0x08); // Display OFF
	send();
	fflush(value[E]);
	usleep(50);
}

// Clears the LCD
void clearDisplay() {
	fprintf(value[RS], "%d", 0);
	fflush(value[RS]);
	fprintf(value[RW], "%d", 0);
	fflush(value[RW]);
	setBus((unsigned char) 0x01); // Clear Display
	send();
	fflush(value[E]);
	usleep(50);
}

// Writes the "character" to the LCD screen ("character" expects the correct
// value based on the char table in the LCD spec)
void writeChar(unsigned char character) {
	fprintf(value[RS], "%d", 1);
	fflush(value[RS]);
	fprintf(value[RW], "%d", 0);
	fflush(value[RW]);
	setBus(character);
	send();
	fflush(value[E]);
	usleep(50);
}

// Sets the R/W pointer to the address specified
void setAddress(unsigned char address) {
	fprintf(value[RS], "%d", 0); // Set DD RAM Address to 0
	fflush(value[RS]);
	fprintf(value[RW], "%d", 0);
	fflush(value[RW]);
	address |= 0x80;
	setBus(address);
	send();
	fflush(value[E]);
	usleep(50);
}

// Sets DB7 to DB0 to the given 8 bits
void setBus(unsigned char byte) {
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

// Reads from the given named pipe and prints the 16-character long
// contents on the specified line of the LCD screen
int printScreen(int fd, int line) {
	// Read in the pre-set number of bytes from pipe
	ssize_t bytesread = 0;
	int bytes = 0;
	char message[SCREEN_SIZE];
	while (bytesread < SCREEN_SIZE) {
		bytes = read(fd, message, SCREEN_SIZE - bytesread);
		if (bytes == -1) {
			if (errno != EINTR) {
				printf("Error on read: %s\n", strerror(errno));
				return -1;
			}
			continue;
		}
		bytesread += bytes;
 	}
	if (!line) {
		setAddress((unsigned char) 0x00);
	} else {
		setAddress((unsigned char) 0x40);
	}
	// Print the string
	int i;
	for (i = 0; i < SCREEN_SIZE; i++) {
		writeChar(message[i]);
	}

	// Reset the cursor to the begining
	setAddress((unsigned char) 0x00);
	return 0;
}

// Turns the cursor off on the LCD screen
void cursorOff() {
	usleep(500);
	fprintf(value[RS], "%d", 0); // Function Set #1
	fflush(value[RS]);
	fprintf(value[RW], "%d", 0);
	fflush(value[RW]);
	setBus((unsigned char) 0x0c);
	send();
	fflush(value[E]);
}
