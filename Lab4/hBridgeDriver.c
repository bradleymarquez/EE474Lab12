/*	ledinit.c
 * Brad Marquez, Joseph Rothlin, Aigerim Shintemirova
 * 22 / April / 2016
 *
 *	
 *  
 */

static FILE* sys, sys2, PWMA_T, PWMA_DUTY, PWMB_T, PWMA_DUTY, RS_VAL, RW_VAL, SER_DATA_VAL;

// CHANGE THESE
#define RS_ 48 // RS Pin - GPIO_PIN_48
#define RW_ 49 // RW Pin - GPIO_PIN_49
#define SER_DATA_ 67 // PIN
#define RS_CLOCK_ 69
#define LATCH_

void pointSetup(void);
int main(){
	pointSetup();
	return 0;
}

void pointSetup(){
	sys = fopen("/sys/devices/bone_capemgr.9/slots", "w");
	fseek(sysA, 0, SEEK_END);
	fprintf(sysA, "am33xx_pwm");
	fflush(sysA);
	fprintf(sysA, "bone_pwm_P9_14"); // need to change???
	fflush(sys);
	
	PWMA_DUTY = fopen("/sys/devices/ocp.3/pwm_test_P9_14.15/duty", "w"); // change directory
	PWMA_T = fopen("/sys/devices/ocp.3/pwm_test_P9_14.15/period", "w"); // change directory
	
	PWMB_DUTY = fopen("/sys/devices/ocp.3/pwm_test_P9_14.15/duty", "w"); // change directory
	PWMB_T = fopen("/sys/devices/ocp.3/pwm_test_P9_14.15/period", "w"); // change directory
	
	changePWMA(10, 20); // what values of PWM?
	changePWMB(10, 20); // what values of PWM?
	
	sys2 = fopen("/sys/class/gpio/export", "w");
	fseek(sys2, 0, SEEK_SET);
	FILE * RS_dir, RW_dir, SER_dir;
	
	RS_dir = fopen("/sys/class/gpio/gpio48/direction", "w"); // change directory
	fseek(RS_dir, 0, SEEK_SET);
	fprintf(RS_dir, "%s", "out");
	fflush(RS_dir);
	
	RW_dir = fopen("/sys/class/gpio/gpio48/direction", "w"); // change directory
	fseek(RW_dir, 0, SEEK_SET);
	fprintf(RW_dir, "%s", "out");
	fflush(RW_dir);
	
	SER_dir = fopen("/sys/class/gpio/gpio48/direction", "w"); // change directory
	fseek(SER_dir, 0, SEEK_SET);
	fprintf(SER_dir, "%s", "out");
	fflush(SER_dir);
	
	RS_VAL = fopen("/sys/class/gpio/gpio48/value", "w"); // change directory
	RW_VAL = fopen("/sys/class/gpio/gpio48/value", "w"); // change directory
	SER_DATA_VAL = fopen("/sys/class/gpio/gpio48/value", "w"); // change directory
}

void changePWMA(int duty, int period) {
	fprintf(PWMA_DUTY, "%d", duty);
	fflush(PWMA_DUTY);
	fprintf(PWMA_T, "%d", period);
	fflush(PWMA_T);
}

void changePWMB(int duty, int period) {
	fprintf(PWMB_DUTY, "%d", duty);
	fflush(PWMB_DUTY);
	fprintf(PWMB_T, "%d", period);
	fflush(PWMB_T);
}

// NEED ORDER OF PINS FROM SHIFT REGISTER TO H-BRIDGE
// calculate command num for drive forward, turn left, turn right, stop, cruise, etc.
void command(unsigned char num) {
	int i = 0;
	int j = 7;
	int input[8];
	int temporary = num;

	// Building the binary version of num
	while (i < 8) {
		input[i] = temporary % 2;
		temporary = temporary >> 1;
		i++;
	}
	
	// Inserting binary value into shift register
	while (j >= 0) {
		fprintf(SER_DATA_VAL, "%d", input[j]);

		// Toggle the clock
		fprintf(RS_CLOCK_, "%d", 1);		
		udelay(10);
		fprintf(RS_CLOCK_, "%d", 0);
		i--;
	}
	
	fprintf(LATCH_, "%d", 1);
	udelay(50);
	fprintf(LATCH_, "%d", 0);
}