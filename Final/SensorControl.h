#ifndef _SENSORCONTROL_H_
#define _SENSORCONTROL_H_

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>

#define RIGHT_SENS_FILE "/sys/bus/iio/devices/iio:device0/in_voltage???_raw"
#define LEFT_SENS_FILE "/sys/bus/iio/devices/iio:device0/in_voltage???_raw"

#define RIGHT true;
#define LEFT false;

FILE *rightSens, *leftSens;

// Initializes the files used to control the two distance sensors
// Returns true on success, false on failure
bool sensorInit();

// Reads the value from the specified sensor
int readSens(int whichSens);

// Closes files that were opened from servoInit()
void closeServo();

#endif  // _SERVOCONTROL_H_
