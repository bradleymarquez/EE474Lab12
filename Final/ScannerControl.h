#ifndef _SCANNERCONTROL_H_
#define _SCANNERCONTROL_H_

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <stdbool.h>
#include <signal.h>

#include "ServoControl.h"
#include "SensorControl.h"

#define NUM_OF_ANGLES 23
#define NUM_OF_SENSORS 2

typedef struct scan_data_struct {
	int distances[NUM_OF_SENSORS][NUM_OF_ANGLES];
} scan_data;

// Initialize devices for the scanner
bool scannerInit();

// Will slowly rotate motor roughly 5 degrees at a time taking a "samples"
// number of readings from the distance sensors "pause" microseconds apart
// after each slight rotation and average the sensor readings from each angle.
// The data will be returned by modifying the scan_data argument.
void scan(int samples, int pause, scan_data *returnData);

// Reads the specified sensor "samples" number of times "pause" microseconds
// apart and returns the average
int getAverage(int samples, int pause, int whichSens);

// Prints the values within the given scan_data to the terminal
void printNums(scan_data *data);

// Prints a visual representation of the given scan_data to the terminal
void printDisplay(scan_data *data);

// Close devices used by the scanner
void closeScanner();

#endif  // _SCANNERCONTROL_H_
