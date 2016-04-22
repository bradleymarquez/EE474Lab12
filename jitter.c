/*	jitter.c
 * Brad Marquez, Joseph Rothlin, Aigerim Shintemirova
 * 10 / April / 2016
 *
 *	
 *  
 */

#include <stdio.h>  // for File IO and printf
#include <unistd.h> // for usleep

#define GPIO_PIN_115 115 // GPIO_PIN_115 @ P9
const int sleepTime = 1; // delay time in microseconds
int main() {

	// Creates pointers to interface with the files of the Beaglebone
	FILE *sys, *dir115, *val115;

	// Selects the files for writing in GPIO digital pins
	sys = fopen("/sys/class/gpio/export", "w");
   
	// Sets the positions of the streams to the beginning for the GPIO digital pin
	fseek(sys, 0, SEEK_SET); 

	// Writes the value corresponding to the GPIO digital pins used
	fprintf(sys, "%d", GPIO_PIN_115);
	fflush(sys);

	// Sets the direction of the GPIO to output
	dir115 = fopen("/sys/class/gpio/gpio115/direction", "w");
	fseek(dir115, 0, SEEK_SET);
	fprintf(dir115, "%s", "out");
	fflush(dir115);

	// Opens the file that controls if the pin is high or low
	val115 = fopen("/sys/class/gpio/gpio115/value", "w");
	fseek(val115, 0, SEEK_SET);


	// Sets GPIO Pin 115 high and low periodically with a period of 2 us
	while(1) 
		fprintf(val115, "%d", 0);
		fflush(val115);
		usleep(sleeptime);
		fprintf(val115," "%d", 1);
		usleep(sleeptime);
	}

    // Closes all accessed files
	fclose(sys);
	fclose(dir115);
	fclose(val115);
	
	// returns 0 if program runs all the way through
	return 0; 
}