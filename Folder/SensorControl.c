/*	SensorControl.c
 * Brad Marquez, Joseph Rothlin, Aigerim Shintemirova
 * 08 / June / 2016
 */

#include "SensorControl.h"

// Initializes the sensor
bool sensorInit() {
	printf("Exit sensorInit\n");
	if (system("echo cape-bone-iio > /sys/devices/bone_capemgr.9/slots") == -1) {
		printf("Error creating ADC files\n");
		return false;
	}
	rightSens = fopen(RIGHT_SENS_FILE, "r");
	leftSens = fopen(LEFT_SENS_FILE, "r");
	frontSens = fopen(FRONT_SENS_FILE, "r");
	backSens = fopen(BACK_SENS_FILE, "r");
	if (rightSens == NULL || leftSens == NULL || frontSens == NULL || backSens == NULL) {
		printf("Error opening ADC pins\n");
		return false;
	}
	printf("Exit sensorInit\n");
	return true;
}

// Reads value of given sensor
int readSens(int whichSens) {
	FILE *curr;
	int ret;
	if (whichSens == RIGHT) {
		curr = rightSens;
	} else if (whichSens == LEFT) {
		curr = leftSens;
	} else if (whichSens == FRONT) {
		curr = frontSens;
	} else {
		curr = backSens;
	}
	fseek(curr, 0, SEEK_SET);
	if (fscanf(curr, "%d", &ret) != 1) {
		printf("Error with fscanf\n");
		return 0;
	}
	fflush(curr);
	return ret;
}

// Closes sensor files
void closeSensor() {
	fclose(rightSens);
	fclose(leftSens);
	fclose(frontSens);
	fclose(backSens);
}
