#include "ScannerControl.h"

// Initialize devices for the scanner
bool scannerInit() {
	return servoInit() && sensorInit();
}

// Will slowly rotate motor roughly 5 degrees at a time taking a "samples"
// number of readings from the distance sensors "pause" microseconds apart
// after each slight rotation and average the sensor readings from each angle.
// The data will be returned by modifying the scan_data argument.
void scan(int samples, int pause, scan_data *returnData) {
	int i;
	for (i = 0; i < NUM_OF_SENSORS; i++) {
		angleFromZero(0);
		(*returnData)[i][0] = getAverage(samples, i);
		int j;
		for (j = 15; j <= 120; j += 5) {
			(*returnData)[i][(j - 10) / 5] = getAverage(samples, pause, j);
		}
	}        
}

// Reads the specified sensor "samples" number of times "pause" microseconds
// apart and returns the average
int getAverage(int samples, int pause, int whichSens) {
	int average = 0;
	int i;
	for (i = 0; i < samples; i++) {
		average += readSens(whichSens) / samples;
		usleep(pause);
	}
	return average;
}

// Prints the values within the given scan_data to the terminal
void printNums(scan_data *data) {
	int i;
	for (i = 0; i < NUM_OF_ANGLES; i++) {
		printf("%d\t:\t%d\n", (*data)[LEFT][i], (*data)[RIGHT][(NUM_OF_ANGLES - 1) - i]);
	}
	printf("\n");
}

// Prints a visual representation of the given scan_data to the terminal
void printDisplay(scan_data *data) {
	
}

// Close devices used by the scanner
void closeScanner() {
	closeServo();
	closeSensor();
}
