/*	hBridgeDriver.c
 * Brad Marquez, Joseph Rothlin, Aigerim Shintemirova
 * 24 / May / 2016
 *
 *	TODO:
 *  - Receive Signal from slave timerinterrupt.cabs
 *  - Automatically stop when timerinterrupt sends signal
 *  - Some kind of control system (button + bluetooth???)
 *  - Change directories of GPIO pins/ PWM pins/
 *  - Figure out hex commands for each motor action
 *  
 */

static FILE* sys, sys2, PWMA_T, PWMA_DUTY, PWMB_T, PWMA_DUTY, RS_VAL, RW_VAL, SER_DATA_VAL, RS_dir, RW_dir, SER_dir;

// CHANGE THESE
#define RS_ 48 // RS Pin - GPIO_PIN_48
#define RW_ 49 // RW Pin - GPIO_PIN_49
#define SER_DATA_ 67 // PIN
#define RS_CLOCK_ 69
#define LATCH_

void pointSetup(void);
void closePointers(void);
void setOut(FILE*);
void changePWMA(int, int);
void changePWMB(int, int);
void command(unsigned char);
int main(){
	pointSetup();
	closePointers();
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
	
	RS_dir = fopen("/sys/class/gpio/gpio48/direction", "w"); // change directory
	setOut(RS_dir);
	
	RW_dir = fopen("/sys/class/gpio/gpio48/direction", "w"); // change directory
	setOut(RW_dir);
	
	SER_dir = fopen("/sys/class/gpio/gpio48/direction", "w"); // change directory
	setOut(SER_dir);
	
	RS_VAL = fopen("/sys/class/gpio/gpio48/value", "w"); // change directory
	RW_VAL = fopen("/sys/class/gpio/gpio48/value", "w"); // change directory
	SER_DATA_VAL = fopen("/sys/class/gpio/gpio48/value", "w"); // change directory
}

void closePointers() {
	fclose(sys);
	fclose(sys2);
	fclose(PWMA_T);
	fclose(PWMA_DUTY);
	fclose(PWMB_T);
	fclose(PWMA_DUTY);
	fclose(RS_VAL);
	fclose(RW_VAL);
	fclose(SER_DATA_VAL);
	fclose(RS_dir);
	fclose(RW_dir);
	fclose(SER_dir);
}

void setOut(FILE *dir) {
	fseek(dir, 0, SEEK_SET);
	fprintf(dir, "%s", "out");
	fflush(dir);
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