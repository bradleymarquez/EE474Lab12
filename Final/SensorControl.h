#ifndef _SENSORCONTROL_H_
#define _SENSORCONTROL_H_

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>

#define FRONT_SENS_FILE "/sys/bus/iio/devices/iio:device0/in_voltage0_raw"
#define LEFT_SENS_FILE "/sys/bus/iio/devices/iio:device0/in_voltage2_raw"
#define BACK_SENS_FILE "/sys/bus/iio/devices/iio:device0/in_voltage4_raw"
#define RIGHT_SENS_FILE "/sys/bus/iio/devices/iio:device0/in_voltage6_raw"

#define FRONT 0
#define BACK 1
#define LEFT 2
#define RIGHT 3

FILE *rightSens, *leftSens, *frontSens, *backSens;

// Initializes the files used to control the two distance sensors
// Returns true on success, false on failure
bool sensorInit();

// Reads the value from the specified sensor
int readSens(int whichSens);

// Closes files that were opened from servoInit()
void closeSensor();

#endif  // _SERVOCONTROL_H_
