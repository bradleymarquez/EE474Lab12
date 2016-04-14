/*	music.c
 * Brad Marquez, Joseph Rothlin, Aigerim Shintemirova
 * 10 / April / 2016
 *
 *	Blinks an external LED from gpio0[8] = x70 pin 27 **************************needs changing ************
 */

#include <stdio.h>  // for File IO and printf
#include <unistd.h> // for usleep
#include "music.h"

#define GPIO_PIN_45 45   // LED pin #1 for the down counter
#define GPIO_PIN_47 47	 // LED pin #2
#define GPIO_PIN_27 27   // LED pin #3

const int sleepTime = 500000; // delay time in microseconds

// period of notes used in nanoseconds
const int noteAb = 2409639;
const int noteA = 2272727;
const int noteF = 2865330;
const int noteC = 1912046;
const int noteE = 1517451;
const int noteF2 = 1432665;
const int noteA2 = 1136364;
const int noteAb2 = 1203934;
const int noteG2 = 1351370;
const int noteGb2 = 1275526;
const int noteBb = 2145186;
const int noteEb = 1607071;
const int noteD = 1702620;
const int noteDb = 1803849;
const int noteB = 2024783;


int main() {

	// Creates pointers to interface with the files of the Beaglebone
	FILE *sys, *sys2, *dir45, *dir115, *dir117, *dirduty, *dirT, *val45, *val47, *val27;

	// Selects the files for writing in GPIO digital pins and the Pulse Width Modulation output
	sys = fopen("/sys/class/gpio/export", "w");
	sys2 = fopen("/sys/devices/bone_capemgr.9/slots", "w");
   
	// Sets the positions of the streams to the beginning for the GPIO digital pins
	// and the end for the Pulse Width Modulation output
	fseek(sys, 0, SEEK_SET); 
	fseek(sys2, 0, SEEK_END);

	// Writes the value corresponding to the GPIO digital pins used
	fprintf(sys, "%d", GPIO_PIN_45);
	fflush(sys);
	fprintf(sys, "%d", GPIO_PIN_47);
	fflush(sys);
	fprintf(sys, "%d", GPIO_PIN_27);
	fflush(sys);
	
	// Writes the PWM used and the corresponding PWM output pin
	fprintf(sys2, "am33xx_pwm");
	fflush(sys2);
	fprintf(sys2, "bone_pwm_P8_19");
	fflush(sys2);

	// Sets the direction of each GPIO to output
	dir45 = fopen("/sys/class/gpio/gpio45/direction", "w");
	fseek(dir45, 0, SEEK_SET);
	fprintf(dir45, "%s", "out");
	fflush(dir45);
	
	dir115 = fopen("/sys/class/gpio/gpio47/direction", "w");
	fseek(dir115, 0, SEEK_SET);
	fprintf(dir115, "%s", "out");
	fflush(dir115);

	dir117 = fopen("/sys/class/gpio/gpio27/direction", "w");
	fseek(dir117, 0, SEEK_SET);
	fprintf(dir117, "%s", "out");
	fflush(dir117);
	
	// Sets the pointers to the appropriate duty and period files
	dirduty = fopen("/sys/devices/ocp.3/pwm_test_P8_19.15/duty", "w");
	dirT = fopen("/sys/devices/ocp.3/pwm_test_P8_19.15/period", "w");

	// Opens the file that controls if the pin is high or low
	val45 = fopen("/sys/class/gpio/gpio45/value", "w");
	fseek(val45, 0, SEEK_SET);

	val47 = fopen("/sys/class/gpio/gpio47/value", "w");
	fseek(val47, 0, SEEK_SET);

	val27 = fopen("/sys/class/gpio/gpio27/value", "w");
	fseek(val27, 0, SEEK_SET);
	
	// Displays the down counter, playing a distinct sound for each count value
	int counter[3] = {0, 0, 0};
	int count = 0;
	int noteSpace = 0;
	while(1) {
		
		// Creates the binary representation of the looping down counter
		int temp0 = counter[0];
		int temp1 = counter[1];
		int temp2 = counter[2];
		counter[0] = ~temp0;
		counter[1] = temp0 ^ temp1;
		counter[2] = (~temp0 & temp2) | (~temp1 & temp2) | (temp0 & temp1 & ~temp2);
		fprintf(val45, "%d", counter[0]);
		fflush(val45);
		fprintf(val47, "%d", counter[1]);
		fflush(val47);
		fprintf(val27, "%d", counter[2]);
		fflush(val27);
		
		// Sets note and duty cycle for the Pulse Width Modulation output
 		// Plays The Imperial March - John Williams
		if (noteSpace == 0) { // Measures # 1-2
			if (count <= 2 || count == 4 || count == 7) { 
				soundChange(dirduty, dirT, noteA, noteA / 2);
				usleep(sleepTime * 7 / 8); // ~1/4 note
				soundChange(dirduty, dirT, 0, 0); 
				usleep(sleepTime / 8); // enunciates seperate notes
			} else if (count == 3 || count == 5) {
				soundChange(dirduty, dirT, noteF, noteF / 2);
				usleep(sleepTime * 3 / 4); // 3/16 note
				soundChange(dirduty, dirT, noteC, noteC / 2);
				usleep(sleepTime / 4); // 1/16 note
			} else {
				soundChange(dirduty, dirT, noteA, noteA / 2);
				usleep(sleepTime); // 1/4 note
			}	
		} else if (noteSpace == 1) { // Measures # 3-4
			if (count <= 2) {
				soundChange(dirduty, dirT, noteE, noteE / 2);
				usleep(sleepTime * 7 / 8);  // ~1/4 note
				soundChange(dirduty, dirT, 0, 0);
				usleep(sleepTime / 8); // enunciates seperate notes
			} else if (count == 3) {
				soundChange(dirduty, dirT, noteF2, noteF2 / 2);
				usleep(sleepTime * 3 / 4); // 3/16 note
				soundChange(dirduty, dirT, noteC, noteC / 2);
				usleep(sleepTime / 4); // 1/16 note
			} else if (count == 4) {
				soundChange(dirduty, dirT, noteAb, noteAb / 2);
				usleep(sleepTime); // 1/4 note
			} else if (count == 5){
				soundChange(dirduty, dirT, noteF, noteF / 2);
				usleep(sleepTime * 3 / 4); // 3/16 note
				soundChange(dirduty, dirT, noteC, noteC / 2);
				usleep(sleepTime / 4); // 1/16 note
			} else if (count == 6) {
				soundChange(dirduty, dirT, noteA, noteA / 2);
				usleep(sleepTime); // 1/4 note
			} else {
				soundChange(dirduty, dirT, noteA, noteA / 2);
				usleep(sleepTime * 7 / 8); // ~1/4 note
				soundChange(dirduty, dirT, 0, 0);
				usleep(sleepTime / 8); // enunciates seperate notes
			}
		} else if (noteSpace == 2) { // Measures # 5-6
			if (count == 0 || count == 2) {
				soundChange(dirduty, dirT, noteA2, noteA2 / 2);
				usleep(sleepTime * 7 / 8); // ~1/4 note
				soundChange(dirduty, dirT, 0, 0);
				usleep(sleepTime / 8); // enunciates seperate notes
			} else if (count == 1) {
				soundChange(dirduty, dirT, noteA, noteA / 2);
				usleep(sleepTime * 3 / 4); // 3/16 note
				soundChange(dirduty, dirT, 0, 0);
				usleep(sleepTime / 8); // 1/32 rest
				soundChange(dirduty, dirT, noteA, noteA / 2);
				usleep(sleepTime / 8); // 1/32 note
			} else if (count == 3) {
				soundChange(dirduty, dirT, noteAb2, noteAb2 / 2);
				usleep(sleepTime * 3 / 4); // 3/16 note
				soundChange(dirduty, dirT, noteG2, noteG2 / 2);
				usleep(sleepTime / 4); // 1/16 note
			} else if (count == 4) {
				soundChange(dirduty, dirT, noteGb2, noteGb2 / 2);
				usleep(sleepTime / 4); // 1/16 note
				soundChange(dirduty, dirT, noteF2, noteF2 / 2);
				usleep(sleepTime / 4); // 1/16 note
				soundChange(dirduty, dirT, noteGb2, noteGb2 / 2);
				usleep(sleepTime / 2); // 1/8 note
			} else if (count == 5) {
				soundChange(dirduty, dirT, 0, 0);
				usleep(sleepTime / 2); // 1/8 note
				soundChange(dirduty, dirT, noteBb, noteBb / 2);
				usleep(sleepTime / 2); // 1/8 note
			} else if (count == 6) {
				soundChange(dirduty, dirT, noteEb, noteEb / 2);
				usleep(sleepTime); // 1/4 note
			} else if (count == 7) {
				soundChange(dirduty, dirT, noteD, noteD / 2);
				usleep(sleepTime / 2); // 1/8 note
				soundChange(dirduty, dirT, noteDb, noteDb / 2);
				usleep(sleepTime / 2);// 1/8 note	
			}
		} else if (noteSpace == 3) { // Measures # 7-8
			if (count == 0) {
				soundChange(dirduty, dirT, noteC, noteC / 2);
				usleep(sleepTime / 4); // 1/16 note
				soundChange(dirduty, dirT, noteB, noteB / 2);
				usleep(sleepTime / 4); // 1/16 note
				soundChange(dirduty, dirT, noteC, noteC / 2);
				usleep(sleepTime / 2); // 1/8 note
			} else if (count == 1) {
				soundChange(dirduty, dirT, 0, 0);
				usleep(sleepTime / 2); // 1/8 note
				soundChange(dirduty, dirT, noteF, noteF / 2);
				usleep(sleepTime / 2); // 1/8 note
			} else if (count == 2) {
				soundChange(dirduty, dirT, noteAb, noteAb / 2);
				usleep(sleepTime); // 1/4 note
			} else if (count == 3) {
				soundChange(dirduty, dirT, noteF, noteF / 2);
				usleep(sleepTime * 3 / 4); // 3/16 note
				soundChange(dirduty, dirT, noteA, noteA / 2);
				usleep(sleepTime / 4); // 1/16 note
			} else if (count == 4) {
				soundChange(dirduty, dirT, noteC, noteC / 2);
				usleep(sleepTime); // 1/4 note
			} else if (count == 5) {
				soundChange(dirduty, dirT, noteA, noteA / 2);
				usleep(sleepTime * 3 / 4); // 3/16 note
				soundChange(dirduty, dirT, noteC, noteC / 2);
				usleep(sleepTime / 4); // 1/16 note
			} else if (count == 6) {
				soundChange(dirduty, dirT, noteE, noteE / 2);
				usleep(sleepTime); // 1/4 note
			} else {
				soundChange(dirduty, dirT, noteE, noteE / 2);
				usleep(sleepTime * 7 / 8); // ~1/4 note
				soundChange(dirduty, dirT, 0, 0);
				usleep(sleepTime / 8); // enunciates seperate notes	
			}
		} else if (noteSpace == 4) { // Measures # 9-10
			if (count == 0 || count == 2) {
				soundChange(dirduty, dirT, noteA2, noteA2 / 2);
				usleep(sleepTime); // 1/4 note
			} else if (count == 1) {
				soundChange(dirduty, dirT, noteA, noteA / 2);
				usleep(sleepTime * 3 / 4); // 3/16 note
				soundChange(dirduty, dirT, 0, 0);
				usleep(sleepTime / 16); // enunciates seperate notes
				soundChange(dirduty, dirT, noteA, noteA / 2);
				usleep(sleepTime * 3 / 16); // ~1/16 note
			} else if (count == 3) {
				soundChange(dirduty, dirT, noteAb2, noteAb2 / 2);
				usleep(sleepTime / 2); // 1/8 note
				soundChange(dirduty, dirT, noteG2, noteG2 / 2);
				usleep(sleepTime / 2); // 1/8 note
			} else if (count == 4) {
				soundChange(dirduty, dirT, noteGb2, noteGb2 / 2);
				usleep(sleepTime / 4); // 1/16 note
				soundChange(dirduty, dirT, noteF2, noteF2 / 2);
				usleep(sleepTime / 4); // 1/16 note
				soundChange(dirduty, dirT, noteGb2, noteGb2 / 2);
				usleep(sleepTime / 2); // 1/8 note
			} else if (count == 5) {
				soundChange(dirduty, dirT, 0, 0);
				usleep(sleepTime / 2); // 1/8 note
				soundChange(dirduty, dirT, noteB, noteB / 2);
				usleep(sleepTime / 2); // 1/8 note
			} else if (count == 6) {
				soundChange(dirduty, dirT, noteEb, noteEb / 2);
				usleep(sleepTime); // 1/4 note
			} else {
				soundChange(dirduty, dirT, noteD, noteD / 2);
				usleep(sleepTime / 2); // 1/8 note
				soundChange(dirduty, dirT, noteDb, noteDb / 2);
				usleep(sleepTime / 2); // 1/8 note
			}
		} else if (noteSpace == 5) { // Measures # 11 - 12
			if (count == 0) {
				soundChange(dirduty, dirT, noteC, noteC / 2);
				usleep(sleepTime / 4); // 1/16 note
				soundChange(dirduty, dirT, noteB, noteB / 2);
				usleep(sleepTime / 4); // 1/16 note
				soundChange(dirduty, dirT, noteC, noteC / 2);
				usleep(sleepTime / 2); // 1/8 note
			} else if (count == 1) {
				soundChange(dirduty, dirT, 0, 0);
				usleep(sleepTime / 2); // 1/8 note
				soundChange(dirduty, dirT, noteF, noteF / 2);
				usleep(sleepTime / 2); // 1/8 note
			} else if (count == 2) {
				soundChange(dirduty, dirT, noteAb, noteAb2 / 2);
				usleep(sleepTime); // 1/4 note
			} else if (count == 3) {
				soundChange(dirduty, dirT, noteF, noteF / 2);
				usleep(sleepTime * 3 / 4); // 3/16 note
				soundChange(dirduty, dirT, noteC, noteC / 2);
				usleep(sleepTime / 4); // 1/16 note
			} else if (count == 4) {
				soundChange(dirduty, dirT, noteA, noteA / 2);
				usleep(sleepTime); // 1/4 note
			} else if (count == 5) {
				soundChange(dirduty, dirT, noteF, noteF / 2);
				usleep(sleepTime * 3 / 4); // 3/16 note
				soundChange(dirduty, dirT, noteC, noteC / 2);
				usleep(sleepTime / 4); // 1/16 note
			} else if (count == 6) {
				soundChange(dirduty, dirT, noteA, noteA / 2);
				usleep(sleepTime); // 1/4 note
			} else {
				soundChange(dirduty, dirT, noteA, noteA / 2);
				usleep(sleepTime * 7 / 8); // ~1/4 note
				soundChange(dirduty, dirT, 0, 0);
				usleep(sleepTime / 8); // enunciates seperate notes
			}
		}
					
		// Increments time counters
		count = (count + 1) % 8;
		if (count == 0) {
			noteSpace = (noteSpace + 1) % 6;
		}
	}

    // Closes all accessed files
	fclose(sys);
	fclose(sys2);
	fclose(dir45);
	fclose(dir115);
	fclose(dir117);
	fclose(dirT);
	fclose(dirduty);
	fclose(val45);
	fclose(val47);
	fclose(val27);
	
	// returns 0 if program runs all the way through
	return 0; 
}

// This function takes two files pointes and two ints. note will be written
// to dirT and dutyc will be written to dirduty. This allows a convenient
// way to change both the duty cycle and frequency of a PWM pin.
void soundChange(FILE *dirduty, FILE *dirT, int note, int dutyc) {
	fprintf(dirduty, "%d", dutyc);
	fflush(dirduty);
	fprintf(dirT, "%d", note);
	fflush(dirT);
}
