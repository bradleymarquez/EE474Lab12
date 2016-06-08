#include "ServoControl.h"

bool servoInit() {
	// For now is just using the same pin that the motor is supposed to use
	printf("Enter servoInit\n");
	sys = fopen("/sys/devices/bone_capemgr.9/slots", "w");
	fseek(sys, 0, SEEK_END);
	fprintf(sys, "am33xx_pwm");
	fflush(sys);
	fprintf(sys, "bone_pwm_P8_13");
	fflush(sys);
	printf("bone\n");
	dirduty = fopen("/sys/devices/ocp.3/pwm_test_P8_13.17/duty", "w");
	printf("duty\n");
	dirT = fopen("/sys/devices/ocp.3/pwm_test_P8_13.17/period", "w");
	printf("T\n");
	fprintf(dirT, "%d", 20000000);
	fflush(dirT);
	printf("Tp\n");
	fprintf(dirduty, "%d", 0);
	fflush(dirduty);
	printf("dutyp\n");
	printf("Exit servoInit\n");
	return true;
}

void angleFromZero(int angle) {
	long duty = ((((long) angle) * 50000) / 4) + 18000000;
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
