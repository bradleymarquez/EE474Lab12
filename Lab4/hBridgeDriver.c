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

static FILE* sys, sys2, PWMA_T, PWMA_DUTY, PWMB_T, PWMA_DUTY, SER_DATA_VAL, SR_CLOCK_VAL, LATCH_VAL, SER_dir, SR_dir, LATCH_dir;

#define SER_DATA_ 45 // PIN
#define SR_CLOCK_ 66
#define LATCH_ 69
#define FRONT 0
#define BACK 1
#define LEFT 2
#define RIGHT 3
#define NUM_SENSORS 4
#define PWM_PERIOD 1000
#include "sensorDriver.h"

char *path = "/root/sensor";

void pointSetup(void);
void closePointers(void);
void setOut(FILE*);
void changePWMA(int, int);
void changePWMB(int, int);
void command(unsigned char);
void close(int);
void handler(int);
void goForward(void);
void goLeft(void);
void goRight(void);
void goBackward(void);
void goStop(void);
int main(){
	signal(SIGUSR1, handler);
	signal(SIGINT, close);
	pointSetup();
	goForward();
	while (1) { // autodrive
	}
	closePointers();
	return 0;
}

void goForward() {
	command(0x15);
	changePWMA(PWM_PERIOD / 2, PWM_PERIOD);
	changePWMB(PWM_PERIOD / 2, PWM_PERIOD);
}

void goBackward() {
	command(0xB);
	changePWMA(PWM_PERIOD / 2, PWM_PERIOD);
	changePWMB(PWM_PERIOD / 2, PWM_PERIOD);
}

void goLeft() {
	command(0x1D);
	changePWMA(PWM_PERIOD / 2, PWM_PERIOD);
	changePWMB(PWM_PERIOD / 10, PWM_PERIOD);
}

void goRight() {
	command(0x17);
	changePWMA(PWM_PERIOD / 10, PWM_PERIOD);
	changePWMB(PWM_PERIOD / 2, PWM_PERIOD);
}

void goStop() {
	command(0x01);
	changePWMA(PWM_PERIOD, PWM_PERIOD);
	changePWMB(PWM_PERIOD, PWM_PERIOD);
}

void close(int signo) {
	if (signo == SIGINT) {
	system("pkill --signal SIGINT sensorDriver");
	changePWMA(0, 1); // stop
	changePWMB(0, 1); // stop
	closePointers();
	}
}

void handler(int signo) {
	if (signo == SIGUSR1) {
		// Creates pipe
		printf("Opening pipe\n");
		int fd = open(path, O_RDWR);
		if (fd == -1) {
			printf("Error open: %s\n", strerror(errno));
			return -1;
		}
		
		// probably uneccessary
		/*
		ssize_t bytesread = 0;
		int bytes = 0;
		char readSensor[NUM_SENSORS];
		while (bytesread < NUM_SENSORS) {
			bytes = read(fd, readSensor, NUM_SENSORS);
			if (bytes == -1) {
				if (errno != EINTR) {
					printf("Error on read: %s\n", strerror(errno));
					return -1;
				}
				continue;
			}
			bytesread += bytes;
		}*/
		
		char readSensor[NUM_SENSORS];
		read(fd, readSensor, NUM_SENSORS);
		
		// Sensor interrupt behavior
		switch(readSensor) {
			case('0000'): // no sensors are high
				goForward();
				break;
			case('0001'): // right is high
				goLeft();
				break;
			case('0010'): // left is high
				goRight();
				break;
			case('0011'): // left and right are high
				goForward();
				break;
			case('0100'): // back is high
				goRight();
				break;
			case('0101'): // back and right are high
				goLeft();
				break;
			case('0110'): // back and left are high
				goForward();
				break;
			case('0111'): // back, left, and right are high
				goForward();
				break;
			case('1000'): // front is high
				goBack();
				break;
			case('1001'): // front and right are high
				goLeft();
				break;
			case('1010'): // front and left are high
				goRight();
				break;
			case('1011'): // front, left, and right are high
				goRight();
				break;
			case('1100'): // front and back are high
				goLeft();
				break;
			case('1101'): // front, back, and right are high
				goLeft();
				break;
			case('1110'): // front, back, and left are high
				goRight();
				break;
			case('1111'): // all are high
				goStop();
				break;
		}
	}
}

void pointSetup(){
	sys = fopen("/sys/devices/bone_capemgr.9/slots", "w");
	fseek(sys, 0, SEEK_END);
	fprintf(sys, "am33xx_pwm");
	fflush(sys);
	fprintf(sys, "bone_pwm_P9_14");
	fflush(sys);
	fprintf(sys, "bone_pwm_P9_16");
	fflush(sys);
	
	PWMA_DUTY = fopen("/sys/devices/ocp.3/pwm_test_P9_14.15/duty", "w");
	PWMA_T = fopen("/sys/devices/ocp.3/pwm_test_P9_14.15/period", "w");
	
	PWMB_DUTY = fopen("/sys/devices/ocp.3/pwm_test_P9_16.15/duty", "w");
	PWMB_T = fopen("/sys/devices/ocp.3/pwm_test_P9_16.15/period", "w");
	
	sys2 = fopen("/sys/class/gpio/export", "w");
	fseek(sys2, 0, SEEK_SET);
	
	SER_dir = fopen("/sys/class/gpio/gpio45/direction", "w");
	setOut(SER_dir);
	
	SR_dir = fopen("/sys/class/gpio/gpio66/direction", "w");
	setOut(SR_dir);
	
	LATCH_dir = fopen("/sys/class/gpio/gpio69/direction", "w");
	setOut(LATCH_dir);
	
	SER_DATA_VAL = fopen("/sys/class/gpio/gpio45/value", "w");
	SR_CLOCK_VAL = fopen("/sys/class/gpio/gpio66/value", "w");
	LATCH_VAL = fopen("/sys/class/gpio/gpio69/value", "w");
}

void closePointers() {
	fclose(sys);
	fclose(sys2);
	fclose(PWMA_T);
	fclose(PWMA_DUTY);
	fclose(PWMB_T);
	fclose(PWMA_DUTY);
	fclose(SER_DATA_VAL);
	fclose(SR_CLOCK_VAL);
	fclose(LATCH_VAL);
	fclose(SER_dir);
	fclose(SR_dir);
	fclose(LATCH_dir);
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
// {A: Standby, B: AIN1 (left motor), C: AIN2 (left motor), D: BIN1 (right motor), E: BIN2 (right motor)} => {000,E,D,C,B,A}
// Forward: 00010101 : 0x15
// Backward: 00001011 : 0xB
// Turn Left: 00011101 : 0x1D
// Turn Right: 00010111 : 0x17
// Stop: 00000001 : 0x1
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
		fprintf(SR_CLOCK_, "%d", 1);		
		udelay(10);
		fprintf(SR_CLOCK_, "%d", 0);
		i--;
	}
	
	fprintf(LATCH_, "%d", 1);
	udelay(50);
	fprintf(LATCH_, "%d", 0);
}