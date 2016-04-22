/*	ledinit.c
 * Brad Marquez, Joseph Rothlin, Aigerim Shintemirova
 * 10 / April / 2016
 *
 *	Blinks three external LEDs to represent each bit in a 3-bit down counter
 *  and plays different notes using pulse width modulation for each stage of the counter
 */

#include <stdio.h>  // for File IO and printf
#include <unistd.h> // for usleep

#define LCD_4 48 // RS Pin - GPIO_PIN_48
#define LCD_5 49 // R/W Pin - GPIO_PIN_49
#define LCD_6 117 // E Pin - GPIO_PIN_117
#define LCD_7 66 // DB0 Pin - GPIO_PIN_66
#define LCD_8 69 // DB1 Pin - GPIO_PIN_69
#define LCD_9 45 // DB2 Pin - GPIO_PIN_45
#define LCD_10 47 // DB3 Pin - GPIO_PIN_47
#define LCD_11 67 // DB4 Pin - GPIO_PIN_67
#define LCD_12 68 // DB5 Pin - GPIO_PIN_68
#define LCD_13 44 // DB6 Pin - GPIO_PIN_44
#define LCD_14 26 // DB7 Pin - GPIO_PIN_26

const int sleepTime = 500000; // delay time in microseconds
int main() {

	// Creates pointers to interface with the files of the Beaglebone
	FILE *sys, *dir4, *dir5, *dir6, *dir7, *dir8, *dir9, *dir10, *dir11, *dir12,
	*dir13, *dir14, *val4, *val5, *val6, *val7, *val8, *val9, *val10, *val11, *val12, *val13, *val14;

	// Selects the files for writing in GPIO digital pins and the Pulse Width Modulation output
	sys = fopen("/sys/class/gpio/export", "w");
   
	// Sets the positions of the streams to the beginning for the GPIO digital pins
	// and the end for the Pulse Width Modulation output
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

	// Sets the direction of each GPIO to output
	dir4 = fopen("/sys/class/gpio/gpio48/direction", "w");
	fseek(dir4, 0, SEEK_SET);
	fprintf(dir4, "%s", "out");
	fflush(dir4);
	
	dir5 = fopen("/sys/class/gpio/gpio49/direction", "w");
	fseek(dir5, 0, SEEK_SET);
	fprintf(dir5, "%s", "out");
	fflush(dir5);

	dir6 = fopen("/sys/class/gpio/gpio117/direction", "w");
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

	// Opens the file that controls if the pin is high or low
	val4 = fopen("/sys/class/gpio/gpio48/value", "w");
	fseek(val4, 0, SEEK_SET);

	val5 = fopen("/sys/class/gpio/gpio49/value", "w");
	fseek(val5, 0, SEEK_SET);
	
	val6 = fopen("/sys/class/gpio/gpio117/value", "w");
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
	
	// hardcode LED init	
	fprintf(val6, "%d", 0);
	fflush(val6);

	usleep(20000);
	
	fprintf(val4, "%d", 0); // Function Set
	fflush(val4);
	fprintf(val5, "%d", 0);
	fflush(val5);
	fprintf(val14, "%d", 0);
	fflush(val14);
	fprintf(val13, "%d", 0);
	fflush(val13);
	fprintf(val12, "%d", 1);
	fflush(val12);
	fprintf(val11, "%d", 1);
	fflush(val11);
	fprintf(val10, "%d", 0);
	fflush(val10);
	fprintf(val9, "%d", 0);
	fflush(val9);
	fprintf(val8, "%d", 0);
	fflush(val8);
	fprintf(val7, "%d", 0);
	fflush(val7);
	fprintf(val6, "%d", 1);
	fflush(val6);
	usleep(10);
	fprintf(val6, "%d", 0);
	fflush(val6);
	
	usleep(5000);
	
	fprintf(val4, "%d", 0); // Function Set #2
	fflush(val4);
	fprintf(val5, "%d", 0);
	fflush(val5);
	fprintf(val14, "%d", 0);
	fflush(val14);
	fprintf(val13, "%d", 0);
	fflush(val13);
	fprintf(val12, "%d", 1);
	fflush(val12);
	fprintf(val11, "%d", 1);
	fflush(val11);
	fprintf(val6, "%d", 1);
	fflush(val6);
	usleep(10);
	fprintf(val6, "%d", 0);
	fflush(val6);
	
	usleep(200);
	
	fprintf(val4, "%d", 0); // Function Set #3
	fflush(val4);
	fprintf(val5, "%d", 0);
	fflush(val5);
	fprintf(val14, "%d", 0);
	fflush(val14);
	fprintf(val13, "%d", 0);
	fflush(val13);
	fprintf(val12, "%d", 1);
	fflush(val12);
	fprintf(val11, "%d", 1);
	fflush(val11);
	fprintf(val6, "%d", 1);
	fflush(val6);
	usleep(10);
	fprintf(val6, "%d", 0);
	fflush(val6);
	
	usleep(500);

	fprintf(val4, "%d", 0); // Function Set #4
	fflush(val4);
	fprintf(val5, "%d", 0);
	fflush(val5);
	fprintf(val14, "%d", 0);
	fflush(val14);
	fprintf(val13, "%d", 0);
	fflush(val13);
	fprintf(val12, "%d", 1);
	fflush(val12);
	fprintf(val11, "%d", 1);
	fflush(val11);
	fprintf(val10, "%d", 1);
	fflush(val10);
	fprintf(val9, "%d", 0);
	fflush(val9);
	fprintf(val6, "%d", 1); // send
	fflush(val6);
	usleep(10);
	fprintf(val6, "%d", 0);
	fflush(val6);
	
	while (busyFlagCheck(dir14, val14)) {
		usleep(50);
	}

	/*fprintf(val4, "%d", 0); // Function Set #5
	fflush(val4);
	fprintf(val5, "%d", 0);
	fflush(val5);
	fprintf(val14, "%d", 0);
	fflush(val14);
	fprintf(val13, "%d", 0);
	fflush(val13);
	fprintf(val12, "%d", 0);
	fflush(val12);
	fprintf(val11, "%d", 1);
	fflush(val11);
	fprintf(val10, "%d", 0);
	fflush(val10);
	fprintf(val9, "%d", 0);
	fflush(val9);
	fprintf(val6, "%d", 1); // send
	fflush(val6);
	usleep(10);
	fprintf(val6, "%d", 0);
	fflush(val6);
	
	usleep(500);

	fprintf(val4, "%d", 0); // Function Set #6
	fflush(val4);
	fprintf(val5, "%d", 0);
	fflush(val5);
	fprintf(val14, "%d", 0);
	fflush(val14);
	fprintf(val13, "%d", 0);
	fflush(val13);
	fprintf(val12, "%d", 0);
	fflush(val12);
	fprintf(val11, "%d", 0);
	fflush(val11);
	fprintf(val10, "%d", 1);
	fflush(val10);
	fprintf(val9, "%d", 1);
	fflush(val9);
	fprintf(val6, "%d", 1); // send
	fflush(val6);
	usleep(10);
	fprintf(val6, "%d", 0);
	fflush(val6);
	
	usleep(500);

	fprintf(val4, "%d", 0); // Function Set #7
	fflush(val4);
	fprintf(val5, "%d", 0);
	fflush(val5);
	fprintf(val14, "%d", 0);
	fflush(val14);
	fprintf(val13, "%d", 0);
	fflush(val13);
	fprintf(val12, "%d", 0);
	fflush(val12);
	fprintf(val11, "%d", 0);
	fflush(val11);
	fprintf(val10, "%d", 0);
	fflush(val10);
	fprintf(val9, "%d", 1);
	fflush(val9);
	fprintf(val8, "%d", 1);
	fflush(val8);
	fprintf(val6, "%d", 1); // send
	fflush(val6);
	usleep(10);
	fprintf(val6, "%d", 0);
	fflush(val6);
	
	usleep(500);
	*/fprintf(val4, "%d", 0); // Display OFF
	fflush(val4);
	fprintf(val5, "%d", 0);
	fflush(val5);
	fprintf(val14, "%d", 0);
	fflush(val14);
	fprintf(val13, "%d", 0);
	fflush(val13);
	fprintf(val12, "%d", 0);
	fflush(val12);
	fprintf(val11, "%d", 0);
	fflush(val11);
	fprintf(val10, "%d", 1);
	fflush(val10);
	fprintf(val9, "%d", 0);
	fflush(val9);
	fprintf(val8, "%d", 0);
	fflush(val8);
	fprintf(val7, "%d", 0);
	fflush(val7);
	fprintf(val6, "%d", 1); // send
	fflush(val6);
	usleep(10);
	fprintf(val6, "%d", 0);
	fflush(val6);
	
	while (busyFlagCheck(dir14, val14)) {
		usleep(50);
	}

	fprintf(val4, "%d", 0); // Clear Display
	fflush(val4);
	fprintf(val5, "%d", 0);
	fflush(val5);
	fprintf(val14, "%d", 0);
	fflush(val14);
	fprintf(val13, "%d", 0);
	fflush(val13);
	fprintf(val12, "%d", 0);
	fflush(val12);
	fprintf(val11, "%d", 0);
	fflush(val11);
	fprintf(val10, "%d", 0);
	fflush(val10);
	fprintf(val9, "%d", 0);
	fflush(val9);
	fprintf(val8, "%d", 0);
	fflush(val8);
	fprintf(val7, "%d", 1);
	fflush(val7);
	fprintf(val6, "%d", 1); // send
	fflush(val6);
	usleep(10);
	fprintf(val6, "%d", 0);
	fflush(val6);
	
	while (busyFlagCheck(dir14, val14)) {
		usleep(50);
	}

	fprintf(val4, "%d", 0); // Enry Mode Set
	fflush(val4);
	fprintf(val5, "%d", 0);
	fflush(val5);
	fprintf(val14, "%d", 0);
	fflush(val14);
	fprintf(val13, "%d", 0);
	fflush(val13);
	fprintf(val12, "%d", 0);
	fflush(val12);
	fprintf(val11, "%d", 0);
	fflush(val11);
	fprintf(val10, "%d", 0);
	fflush(val10);
	fprintf(val9, "%d", 1);
	fflush(val9);
	fprintf(val8, "%d", 1);
	fflush(val8);
	fprintf(val7, "%d", 0);
	fflush(val7);
	fprintf(val6, "%d", 1); // send
	fflush(val6);
	usleep(10);
	fprintf(val6, "%d", 0);
	fflush(val6);
	
	while (busyFlagCheck(dir14, val14)) {
		usleep(50);
	}

	fprintf(val4, "%d", 0); // Display ON
	fflush(val4);
	fprintf(val5, "%d", 0);
	fflush(val5);
	fprintf(val14, "%d", 0);
	fflush(val14);
	fprintf(val13, "%d", 0);
	fflush(val13);
	fprintf(val12, "%d", 0);
	fflush(val12);
	fprintf(val11, "%d", 0);
	fflush(val11);
	fprintf(val10, "%d", 1);
	fflush(val10);
	fprintf(val9, "%d", 1);
	fflush(val9);
	fprintf(val8, "%d", 1);
	fflush(val8);
	fprintf(val7, "%d", 0);
	fflush(val7);
	fprintf(val6, "%d", 1); // send
	fflush(val6);
	usleep(10);
	fprintf(val6, "%d", 0);
	fflush(val6);
	
	// Displays the down counter, playing a distinct sound for each count value
	while(1) {
	}

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

int busyFlagCheck(FILE *dir14, FILE *val14) {
	fseek(dir14, 0, SEEK_SET);
	fprintf(dir14, "%s", "in");
	fflush(dir14);
	
	fprintf(val4, "%d", 0); // Reads busy flag
	fflush(val4);
	fprintf(val5, "%d", 1);
	fflush(val5);
	fprintf(val6, "%d", 1); // send
	fflush(val6);
	usleep(10);
	fprintf(val6, "%d", 0);
	fflush(val6);
	
	usleep(50);
	
	int flag = fscanf(val14, "%d");
	
	fprintf(dir14, "%s", "out");
	fflush(dir14);
	
	return flag;
}
