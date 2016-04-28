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
#include <fcntl.h> // for file access modes
#include <string.h> // for strings in fifo
#include <sys/stat.h> // for mkfifo
#include <sys/types.h> // for mkfifo

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
#define TEST 112

static volatile int keepRunning = 1;

void setBus(unsigned char byte, FILE* lcdPins[]);
void setAddress(unsigned char address, FILE* lcdPins[]);
void writeChar(unsigned char character, FILE* lcdPins[]);
void closeLCD(FILE *lcdPins[], FILE *);
void initialize(FILE *lcdPins[], FILE *);
void sigHandler(int);
void send(FILE *);
void printFIFO(unsigned char pipeName[]);
void writeString(unsigned char characters[], FILE* lcdPins[]);
int main() {

	// Creates pointers to interface with the files of the Beaglebone
	FILE *sys, *dir4, *dir5, *dir6, *dir7, *dir8, *dir9, *dir10, *dir11, *dir12,
	*dir13, *dir14, *dirTest, *val4, *val5, *val6, *val7, *val8, *val9, *val10, *val11, *val12, *val13, *val14, *test;

	// Selects the files for writing in GPIO digital pins and the Pulse Width Modulation output
	sys = fopen("/sys/class/gpio/export", "w");
	fseek(sys, 0, SEEK_SET); 

	// Writes the value corresponding to the GPIO digital pins used
	fprintf(sys, "%d", LCD_4);
	fflush(sys);
	fprintf(sys, "%d", LCD_5);
	fflush(sys);
	fprintf(sys, "%d", LCD_6);
	fflush(sys);
	fprintf(sys, "%d", LCD_7);
	fflush(sys);
	fprintf(sys, "%d", LCD_8);
	fflush(sys);
	fprintf(sys, "%d", LCD_9);
	fflush(sys);
	fprintf(sys, "%d", LCD_10);
	fflush(sys);
	fprintf(sys, "%d", LCD_11);
	fflush(sys);
	fprintf(sys, "%d", LCD_12);
	fflush(sys);
	fprintf(sys, "%d", LCD_13);
	fflush(sys);
	fprintf(sys, "%d", LCD_14);
	fflush(sys);
	fprintf(sys, "%d", TEST);
	fflush(sys);

	// Sets the direction of each GPIO to output
	dir4 = fopen("/sys/class/gpio/gpio48/direction", "w");
	fseek(dir4, 0, SEEK_SET);
	fprintf(dir4, "%s", "out");
	fflush(dir4);
	
	dir5 = fopen("/sys/class/gpio/gpio49/direction", "w");
	fseek(dir5, 0, SEEK_SET);
	fprintf(dir5, "%s", "out");
	fflush(dir5);

	dir6 = fopen("/sys/class/gpio/gpio60/direction", "w");
	fseek(dir6, 0, SEEK_SET);
	fprintf(dir6, "%s", "out");
	fflush(dir6);
	
	dir7 = fopen("/sys/class/gpio/gpio66/direction", "w");
	fseek(dir7, 0, SEEK_SET);
	fprintf(dir7, "%s", "out");
	fflush(dir7);
	
	dir8 = fopen("/sys/class/gpio/gpio69/direction", "w");
	fseek(dir8, 0, SEEK_SET);
	fprintf(dir8, "%s", "out");
	fflush(dir8);

	dir9 = fopen("/sys/class/gpio/gpio45/direction", "w");
	fseek(dir9, 0, SEEK_SET);
	fprintf(dir9, "%s", "out");
	fflush(dir9);
	
	dir10 = fopen("/sys/class/gpio/gpio47/direction", "w");
	fseek(dir10, 0, SEEK_SET);
	fprintf(dir10, "%s", "out");
	fflush(dir10);
	
	dir11 = fopen("/sys/class/gpio/gpio67/direction", "w");
	fseek(dir11, 0, SEEK_SET);
	fprintf(dir11, "%s", "out");
	fflush(dir11);

	dir12 = fopen("/sys/class/gpio/gpio68/direction", "w");
	fseek(dir12, 0, SEEK_SET);
	fprintf(dir12, "%s", "out");
	fflush(dir12);
	
	dir13= fopen("/sys/class/gpio/gpio44/direction", "w");
	fseek(dir13, 0, SEEK_SET);
	fprintf(dir13, "%s", "out");
	fflush(dir13);
	
	dir14 = fopen("/sys/class/gpio/gpio26/direction", "w");
	fseek(dir14, 0, SEEK_SET);
	fprintf(dir14, "%s", "out");
	fflush(dir14);

	dirTest = fopen("/sys/class/gpio/gpio112/direction", "w");
	fseek(dirTest, 0, SEEK_SET);
	fprintf(dirTest, "%s", "out");
	fflush(dirTest);

	// Opens the file that controls if the pin is high or low
	val4 = fopen("/sys/class/gpio/gpio48/value", "w");
	fseek(val4, 0, SEEK_SET);

	val5 = fopen("/sys/class/gpio/gpio49/value", "w");
	fseek(val5, 0, SEEK_SET);
	
	val6 = fopen("/sys/class/gpio/gpio60/value", "w");
	fseek(val6, 0, SEEK_SET);

	val7 = fopen("/sys/class/gpio/gpio66/value", "w");
	fseek(val7, 0, SEEK_SET);
	
	val8 = fopen("/sys/class/gpio/gpio69/value", "w");
	fseek(val8, 0, SEEK_SET);

	val9 = fopen("/sys/class/gpio/gpio45/value", "w");
	fseek(val9, 0, SEEK_SET);
	
	val10 = fopen("/sys/class/gpio/gpio47/value", "w");
	fseek(val10, 0, SEEK_SET);

	val11 = fopen("/sys/class/gpio/gpio67/value", "w");
	fseek(val11, 0, SEEK_SET);
	
	val12 = fopen("/sys/class/gpio/gpio68/value", "w");
	fseek(val12, 0, SEEK_SET);

	val13 = fopen("/sys/class/gpio/gpio44/value", "w");
	fseek(val13, 0, SEEK_SET);
	
	val14 = fopen("/sys/class/gpio/gpio26/value", "w");
	fseek(val14, 0, SEEK_SET);

	test = fopen("/sys/class/gpio/gpio112/value", "w");
	fseek(test, 0, SEEK_SET);
	
	fprintf(test, "%d", 0);
	fflush(test);

	FILE* lcdPins[11] = {val4, val5, val6, val7, val8, val9, val10, val11, val12, val13, val14};	

	signal(SIGINT, sigHandler);
	initialize(lcdPins, dir14);

	usleep(1000);

	// Write 0
	setAddress((unsigned char) 0x00, lcdPins);

	usleep(1000);
	writeChar((unsigned char) 0x30, lcdPins);
	
	fprintf(test, "%d", 1);
	fflush(test);	
	
	while(keepRunning) {
	}
	
	closeLCD(lcdPins, dir14);
    // Closes all accessed files
	fclose(sys);
	fclose(dir4);
	fclose(dir5);
	fclose(dir6);
	fclose(dir7);
	fclose(dir8);
	fclose(dir9);
	fclose(dir10);
	fclose(dir11);
	fclose(dir12);
	fclose(dir13);
	fclose(dir14);
	fclose(val4);
	fclose(val5);
	fclose(val6);
	fclose(val7);
	fclose(val8);
	fclose(val9);
	fclose(val10);
	fclose(val11);
	fclose(val12);
	fclose(val13);
	fclose(val14);
	
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


void closeLCD(FILE *lcdPins[], FILE *dir14) {
	usleep(500);
	
	fprintf(lcdPins[RS], "%d", 0); // Function Set #1
	fflush(lcdPins[RS]);
	fprintf(lcdPins[RW], "%d", 0);
	fflush(lcdPins[RW]);
	usleep(500);
	setBus((unsigned char) 0x08, lcdPins); // Display OFF
	send(lcdPins[E]);
	fflush(lcdPins[E]);

	usleep(500);

	setBus((unsigned char) 0x01, lcdPins); // Clear Display
	send(lcdPins[E]);
	fflush(lcdPins[E]);
}

void initialize(FILE* lcdPins[], FILE *dir14) {
	usleep(15001);
	
	fprintf(lcdPins[RS], "%d", 0); // Function Set #1
	fflush(lcdPins[RS]);
	fprintf(lcdPins[RW], "%d", 0);
	fflush(lcdPins[RW]);
	setBus((unsigned char) 0x30, lcdPins);
	send(lcdPins[E]);
	fflush(lcdPins[E]);

	usleep(4101);
	
	send(lcdPins[E]); // Function Set #4
	fflush(lcdPins[E]);
	
	usleep(101);
	
	send(lcdPins[E]); // Function Set #3
	fflush(lcdPins[E]);
	
	usleep(500);

	setBus((unsigned char) 0x38, lcdPins); // Function Set #4
	send(lcdPins[E]);
	fflush(lcdPins[E]);
	
	usleep(500);

	setBus((unsigned char) 0x08, lcdPins); // Display OFF
	send(lcdPins[E]);
	fflush(lcdPins[E]);

	usleep(500);

	setBus((unsigned char) 0x01, lcdPins); // Clear Display
	send(lcdPins[E]);
	fflush(lcdPins[E]);
	
	usleep(500);

	setBus((unsigned char) 0x0c, lcdPins); // Entry Mode Set
	send(lcdPins[E]);
	fflush(lcdPins[E]);
	
	usleep(500);

	setBus((unsigned char) 0x0f, lcdPins);
	send(lcdPins[E]);
	fflush(lcdPins[E]);

	usleep(500);
}

// Flips designated "send" GPIO from on to off, which signals the LCD to take in an instruction
void send(FILE *val6) {
	fprintf(val6, "%d", 1);
	fflush(val6);
	usleep(10);
	fprintf(val6, "%d", 0);
	fflush(val6);
	usleep(10);
}

void printFIFO(unsigned char pipeName[]){
	char out[20]= "FIFO's are fun!", in[20];
	int rfd;
	int wfd;

	if (mkfifo(pipeName, S_IRWXU) != 0) {
		perror("mkfifo() error");
	} else {
		if ((rfd = open(pipeName, O_RDONLY|O_NONBLOCK)) < 0)
		  perror("open() error for read end");
		else {
		  if ((wfd = open(pipeName, O_WRONLY)) < 0)
			perror("open() error for write end");
		  else {
			if (write(wfd, out, strlen(out)+1) == -1)
			  perror("write() error");
			else if (read(rfd, in, sizeof(in)) == -1)
			  perror("read() error");
			else printf("read '%s' from the FIFO\n", in);
			close(wfd);
		  }
		  close(rfd);
		}
		unlink(pipeName);
	  }
}

void writeString(unsigned char characters[], FILE* lcdPins[]){
	if ((unsigned) strlen(characters) < 0x21) {
		int i;
		for (int i = 0; i < (unsigned) strlen(characters); i = i + 1) {
			writeChar(characters[i], lcdPins);
		}
	} else {
		writeString("ERROR - TOO MANY CHARACTERS", lcdPins);
	}
}
void writeChar(unsigned char character, FILE* lcdPins[]) {
	fprintf(lcdPins[RS], "%d", 1);
	fflush(lcdPins[RS]);
	fprintf(lcdPins[RW], "%d", 0);
	fflush(lcdPins[RW]);
	setBus(character, lcdPins);
	send(lcdPins[E]);
	fflush(lcdPins[E]);
	usleep(50);
}

void setAddress(unsigned char address, FILE* lcdPins[]) {
	fprintf(lcdPins[RS], "%d", 0); // Set DD RAM Address to 0
	fflush(lcdPins[RS]);
	fprintf(lcdPins[RW], "%d", 0);
	fflush(lcdPins[RW]);
	address |= 0x80;
	setBus(address, lcdPins);
	send(lcdPins[E]);
	fflush(lcdPins[E]);
	usleep(50);
}

void setBus(unsigned char byte, FILE* lcdPins[]) {
	fprintf(lcdPins[DB0], "%d", (byte % 2));
	fflush(lcdPins[DB0]);
	byte  = byte >> 1;
	fprintf(lcdPins[DB1], "%d", (byte % 2));
	fflush(lcdPins[DB1]);
	byte  = byte >> 1;
	fprintf(lcdPins[DB2], "%d", (byte % 2));
	fflush(lcdPins[DB2]);
	byte  = byte >> 1;
	fprintf(lcdPins[DB3], "%d", (byte % 2));
	fflush(lcdPins[DB3]);
	byte  = byte >> 1;
	fprintf(lcdPins[DB4], "%d", (byte % 2));
	fflush(lcdPins[DB4]);
	byte  = byte >> 1;
	fprintf(lcdPins[DB5], "%d", (byte % 2));
	fflush(lcdPins[DB5]);
	byte  = byte >> 1;
	fprintf(lcdPins[DB6], "%d", (byte % 2));
	fflush(lcdPins[DB6]);
	byte  = byte >> 1;
	fprintf(lcdPins[DB7], "%d", (byte % 2));
	fflush(lcdPins[DB7]);
}

