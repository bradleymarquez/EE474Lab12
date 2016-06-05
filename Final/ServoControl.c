#include "ServoControl.h"

bool servoInit() {
	// For now is just using the same pin that the motor is supposed to use
	sys = fopen("/sys/devices/bone_capemgr.9/slots", "w");
	fseek(sys, 0, SEEK_END);
	fprintf(sys, "am33xx_pwm");
	fflush(sys);
	fprintf(sys, "bone_pwm_P9_14");
	fflush(sys);
	dirduty = fopen("/sys/devices/ocp.3/pwm_test_P9_14.15/duty", "w");
	dirT = fopen("/sys/devices/ocp.3/pwm_test_P9_14.15/period", "w");
	fprintf(dirT, "%d", 20000000);
	fflush(dirT);
	fprintf(dirduty, "%d", 0);
	fflush(dirduty);
	return true;
}

void angleFromZero(int angle) {
	long duty = ((((long) angle) * 50000) / 4) + 18000000;
	printf("%lu\n", duty);
	fprintf(dirduty, "%lu", duty);
	fflush(dirduty);
}

void closeServo() {
	fprintf(dirT, "%d", 0);
	fflush(dirT);
	fprintf(dirduty, "%d", 0);
	fflush(dirduty);
	fclose(sys);
	fclose(dirduty);
	fclose(dirT);
}
