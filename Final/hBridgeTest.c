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
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <error.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#define SER_DATA_ 45 // PIN
#define SR_CLOCK_ 66
#define LATCH_ 69
#define FRONT 0
#define BACK 1
#define LEFT 2
#define RIGHT 3
#define NUM_SENSORS 4
#define PWM_PERIOD 500

static FILE *sys, *sys2, *PWM_T, *PWM_DUTY, *SER_DATA_VAL, *SR_CLOCK_VAL, *LATCH_VAL, *SER_dir, *SR_dir, *LATCH_dir;
char *path = "/tmp/sensor";

void pointSetup(void);
void closePointers(void);
void setOut(FILE*);
void changePWM(int, int);
void command(unsigned char);
void closeHandler(int);
void handler(int);
void goForward(void);
void goLeft(void);
void goRight(void);
void goBackward(void);
void goStop(void);
int main(){
	signal(SIGUSR1, handler);
	signal(SIGINT, closeHandler);	
	pointSetup();	
	goForward();	
	while (1); // autodrive
	closePointers();
	return 0;
}

void goForward() {
	command(0x15);
	//changePWM(PWM_PERIOD / 2, PWM_PERIOD);
	changePWM(0, PWM_PERIOD);
}

void goBackward() {
	command(0xB);
	//changePWM(PWM_PERIOD / 2, PWM_PERIOD);
	changePWM(0, PWM_PERIOD);
}

void goLeft() {
	command(0xD);
	//changePWM(PWM_PERIOD / 2, PWM_PERIOD);
	changePWM(0, PWM_PERIOD);
}

void goRight() {
	command(0x13);
	//changePWM(PWM_PERIOD / 10, PWM_PERIOD);
	changePWM(0, PWM_PERIOD);
}

void goStop() {
	command(0x01);
	//changePWM(PWM_PERIOD, PWM_PERIOD);
	changePWM(0, PWM_PERIOD);
}

void closeHandler(int signo) {
	if (signo == SIGINT) {
		changePWM(0, 1); // stop
		system("pkill --signal SIGINT sensor");
		closePointers();
	}
}

void handler(int signo) {
	if (signo == SIGUSR1) {
		printf("Received Signal: ");
		fflush(stdout); 
		// Creates pipe
		// printf("Opening pipe\n");
		int fd = open(path, O_RDWR);
		if (fd == -1) {
			printf("Error open: %s\n", strerror(errno));
		} else {
			char readSensor[NUM_SENSORS];
			read(fd, readSensor, NUM_SENSORS);
		
			int j;
			for (j = 0; j < 4; j++) {
				printf("%c", readSensor[j]);
			}
			printf("\n");
			fflush(stdout);
			printf("Command Executed: ");
			// Sensor interrupt behavior
			if (strcmp(readSensor, "0000") == 0) { // none high
				goForward();
				printf("0000 - Forward");
			} else if (strcmp(readSensor, "0001") == 0) { // right is high
				goLeft();
				printf("0001 - Left");	
			} else if (strcmp(readSensor, "0010") == 0) { // left is high
				goRight();
				printf("0010 - Right");
			} else if (strcmp(readSensor, "0011") == 0) { // left and right are high
				goForward();
				printf("0011 - Forward");
			} else if (strcmp(readSensor, "0100") == 0) { // front is high
				goBackward();
				printf("0100 - Backward");
			} else if (strcmp(readSensor, "0101") == 0) { // back and right are high
				goLeft();
				printf("0101 - Left");
			} else if (strcmp(readSensor, "0110") == 0) { // back and left are high
				goForward();
				printf("0110 - Forward");
			} else if (strcmp(readSensor, "0111") == 0) { // back, left, and right are high
				goForward();
				printf("0111 - Forward");
			} else if (strcmp(readSensor, "1000") == 0) { // back is high
				goForward();
				printf("1000 - Forward");
			} else if (strcmp(readSensor, "1001") == 0) { // front and right are high
				goLeft();
				printf("1001 - Left");
			} else if (strcmp(readSensor, "1010") == 0) { // front and left are high
				goRight();
				printf("1010 - Right");
			} else if (strcmp(readSensor, "1011") == 0) { // front, left, and right are high
				goBackward();
				printf("1011 - Backward");
			} else if (strcmp(readSensor, "1100") == 0) { // front and back are high
				goRight();
				printf("1100 - Right");
			} else if (strcmp(readSensor, "1101") == 0) { // front, back, and right are high
				goLeft();
				printf("1101 - Left");
			} else if (strcmp(readSensor, "1110") == 0) { // front, back, and left are high
				goRight();
				printf("1110 - Right");
			} else if (strcmp(readSensor, "1111") == 0) { // all are high
				goStop();
				printf("1111 - Stop");
			}
			printf("\n\n");
			fflush(stdout); 
		}
	}
}

void pointSetup(){
	sys = fopen("/sys/devices/bone_capemgr.9/slots", "w");
	if (sys == NULL) {
		printf("Error opening period file: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	fseek(sys, 0, SEEK_END);
	fprintf(sys, "am33xx_pwm");
	fflush(sys);
	fprintf(sys, "bone_pwm_P9_14");
	fflush(sys);
	PWM_DUTY = fopen("/sys/devices/ocp.3/pwm_test_P9_14.16/duty", "w");
	if (PWM_DUTY == NULL) {
		printf("Error opening duty file: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	PWM_T = fopen("/sys/devices/ocp.3/pwm_test_P9_14.16/period", "w");
	if (PWM_T == NULL) {
		printf("Error opening period file: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	sys2 = fopen("/sys/class/gpio/export", "w");
	fseek(sys2, 0, SEEK_SET);

	// Writes the value corresponding to the GPIO digital pins used
	fprintf(sys2, "%d", SER_DATA_);
	fflush(sys2);
	fprintf(sys2, "%d", SR_CLOCK_);
	fflush(sys2);
	fprintf(sys2, "%d", LATCH_);
	fflush(sys2);
	
	SER_dir = fopen("/sys/class/gpio/gpio45/direction", "w");
	setOut(SER_dir);
	SR_dir = fopen("/sys/class/gpio/gpio66/direction", "w");
	setOut(SR_dir);
	LATCH_dir = fopen("/sys/class/gpio/gpio69/direction", "w");
	setOut(LATCH_dir);
	SER_DATA_VAL = fopen("/sys/class/gpio/gpio45/value", "w");
	fseek(SER_DATA_VAL, 0, SEEK_SET);
	SR_CLOCK_VAL = fopen("/sys/class/gpio/gpio66/value", "w");
	fseek(SR_CLOCK_VAL, 0, SEEK_SET);
	LATCH_VAL = fopen("/sys/class/gpio/gpio69/value", "w");
	fseek(LATCH_VAL, 0, SEEK_SET);
}

void closePointers() {
	fclose(sys);
	fclose(sys2);
	fclose(PWM_T);
	fclose(PWM_DUTY);
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

void changePWM(int duty, int period) {
	fprintf(PWM_DUTY, "%d", duty);
	fflush(PWM_DUTY);
	fprintf(PWM_T, "%d", period);
	fflush(PWM_T);
}

// NEED ORDER OF PINS FROM SHIFT REGISTER TO H-BRIDGE
// calculate command num for drive forward, turn left, turn right, stop, cruise, etc.
// {A: Standby, B: AIN1 (left motor), C: AIN2 (left motor), D: BIN1 (right motor), E: BIN2 (right motor)} => {000,E,D,C,B,A}
// Forward: 00010101 : 0x15
// Backward: 00001011 : 0xB
// Turn Left: 00001101 : 0xD
// Turn Right: 00010011 : 0x13
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
		fflush(SER_DATA_VAL);
		// Toggle the clock
		fprintf(SR_CLOCK_VAL, "%d", 1);	
		fflush(SR_CLOCK_VAL);	
		usleep(10);
		fprintf(SR_CLOCK_VAL, "%d", 0);
		fflush(SR_CLOCK_VAL);	
		j--;
	}


	fprintf(LATCH_VAL, "%d", 1);
	fflush(LATCH_VAL);
	usleep(50);
	fprintf(LATCH_VAL, "%d", 0);
	fflush(LATCH_VAL);

}
