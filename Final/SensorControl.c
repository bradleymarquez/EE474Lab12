#include "SensorControl.h"

bool sensorInit() {
	if (system("echo cape-bone-iio > /sys/devices/bone_capemgr.9/slots") == -1) {
		printf("Error creating ADC files\n");
		return false;
	}
	rightSens = fopen(RIGHT_SENS_FILE, "r");
	leftSens = fopen(LEFT_SENS_FILE, "r");
	if (rightSens == NULL || leftSens == NULL) {
		printf("Error opening ADC pins\n");
		return false;
	}
	return true;
}

int readSens(int whichSens) {
	FILE *curr;
	int ret;
	if (whichSens == RIGHT) {
		curr = rightSens;
	} else {
		curr = leftSens;
	}
	fseek(curr, 0, SEEK_SET);
	if (fscanf(curr, "%d", &ret) != 1) {
		printf("Error with fscanf\n");
		return 0;
	}
	return ret;
}

void closeSensor() {
	fclose(rightSens);
	fclose(leftSens);
}
