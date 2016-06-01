/*	hBridgeDriver.c
 * Brad Marquez, Joseph Rothlin, Aigerim Shintemirova
 * 24 / May / 2016
 *
 *
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
#include <stdbool.h>
#include "arduino-serial/arduino-serial-lib.c"

#define SER_DATA_ 45 // Serial Data (Shift Register) - GPIO_PIN_45
#define SR_CLOCK_ 66 // SR Clock (Shift Register) - GPIO_PIN_66
#define LATCH_ 69 // Latch (Shift Register) - GPIO_PIN_69
#define RX_ 68 // RX of Beaglebone - GPIO_PIN_68
#define TX_ 67 // TX of Beaglebone - GPIO_PIN_67
#define FRONT 0 // Front Sensor Index
#define BACK 1 // Back Sensor Index
#define LEFT 2 // Left Sensor Index
#define RIGHT 3 // Right Sensor Index
#define NUM_SENSORS 4 // Sensor Array Size
#define PWM_PERIOD 10000000 // in nanoseconds

static FILE *sys, *sys2, *PWM_T, *PWM_DUTY, *SER_DATA_VAL, *SR_CLOCK_VAL, *LATCH_VAL, *SER_dir, *SR_dir, *LATCH_dir, *TX_dir, *RX_dir, *TX_VAL, *RX_VAL;
char *path = "/tmp/sensor"; // FIFO
bool setup = false;

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
	signal(SIGINT, closeHandler); // Ctrl+C Interrupt
	signal(SIGUSR1, handler); // Sensor Interrupt
	pointSetup(); // Sets up Pointers
	// goForward(); // Default behavior
	while (1); // Autodrive
	return 0;
}

// Commands the H-Bridge to drive both motors CW
void goForward() {
	command(0x15);
	changePWM(0, PWM_PERIOD);
	//changePWM(0, PWM_PERIOD);
}

// Commands the H-bridge to drive both motors CCW
void goBackward() {
	command(0xB);
	changePWM(0, PWM_PERIOD);
	//changePWM(0, PWM_PERIOD);
}

// Commands the H-bridge to drive the "left motor" CCW
// and the "right motor" CW
void goLeft() {
	command(0xD);
	changePWM(0, PWM_PERIOD);
	//changePWM(0, PWM_PERIOD);
}

// Commands the H-bridge to drive the "right motor" CCW
// and the "left motor" CW
void goRight() {
	command(0x13);
	changePWM(0, PWM_PERIOD);
	//changePWM(0, PWM_PERIOD);f
}

// Commands the H-bridge to resist motion on both motors
void goStop() {
	command(0x01);
	changePWM(PWM_PERIOD, PWM_PERIOD);
	//changePWM(0, PWM_PERIOD);
}

// Turns off motors, signals sensor program to terminate and
// closes opened file pointers
void closeHandler(int signo) {
	if (signo == SIGINT) {
		changePWM(0, 1); // stop
		system("pkill --signal SIGINT sensor");
		closePointers();
		exit(EXIT_SUCCESS);
	}
}

// Receives interrupts from sensor program and commands the H-bridge
// based on the information sent by the slave
void handler(int signo) {
	if (signo == SIGUSR1) {
		if (setup) {
			// Opens pipe
			int fd = open(path, O_RDWR);
			if (fd == -1) {
				printf("Error open: %s\n", strerror(errno));
			} else {
				int j;
				char readSensor[NUM_SENSORS];
				read(fd, readSensor, NUM_SENSORS);
				printf("Received Signal: ");
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
				} else if (strcmp(readSensor, "0101") == 0) { // front and right are high
					goLeft();
					printf("0101 - Left");
				} else if (strcmp(readSensor, "0110") == 0) { // front and left are high
					goRight();
					printf("0110 - Right");
				} else if (strcmp(readSensor, "0111") == 0) { // front, left, and right are high
					goBackward();
					printf("0111 - Backward");
				} else if (strcmp(readSensor, "1000") == 0) { // back is high
					goForward();
					printf("1000 - Forward");
				} else if (strcmp(readSensor, "1001") == 0) { // back and right are high
					goLeft();
					printf("1001 - Left");
				} else if (strcmp(readSensor, "1010") == 0) { // back and left are high
					goRight();
					printf("1010 - Right");
				} else if (strcmp(readSensor, "1011") == 0) { // back, left, and right are high
					goForward();
					printf("1011 - Forward");
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
}

void toggleBluetoothPort() {
    if( fd!=-1 ) {
        serialport_close(fd);
        if(!quiet) printf("closed port %s\n",serialport);
    }
    strcpy(serialport,optarg);
    fd = serialport_init(optarg, baudrate);
    if( fd==-1 ) error("couldn't open port");
    if(!quiet) printf("opened port %s\n",serialport);
    serialport_flush(fd);
}

// Sets up access to the PWM pin and GPIO pins
void pointSetup(){
	sys = fopen("/sys/devices/bone_capemgr.9/slots", "w");
	if (sys == NULL) {
		printf("Error opening sys file: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	fseek(sys, 0, SEEK_END);
	
	// Create UART file
	fprintf(sys, "BB_UART4");
	fflush(sys);

	// Creates PWM files
	fprintf(sys, "am33xx_pwm");
	fflush(sys);
	fprintf(sys, "bone_pwm_P9_14");
	fflush(sys);


	// Opens PWM file pointers
	PWM_T = NULL;
	while (PWM_T == NULL) {
		PWM_T = fopen("/sys/devices/ocp.3/pwm_test_P9_14.15/period", "w");
		if (PWM_T == NULL) {
			//fprintf(sys, "bone_pwm_P9_14");
			//fflush(sys);
			PWM_T = fopen("/sys/devices/ocp.3/pwm_test_P9_14.16/period", "w");
			/*if (PWM_T == NULL) {
				printf("Error opening period file %s\n", strerror(errno));
				exit(EXIT_FAILURE);
			}*/
		}
	}

	PWM_DUTY = NULL;
	while (PWM_DUTY == NULL) {
		PWM_DUTY = fopen("/sys/devices/ocp.3/pwm_test_P9_14.15/duty", "w");
		if (PWM_DUTY == NULL) {
			//fprintf(sys, "bone_pwm_P9_14");
			//fflush(sys);
			PWM_DUTY = fopen("/sys/devices/ocp.3/pwm_test_P9_14.16/duty", "w");
			/*if (PWM_DUTY == NULL) {
				printf("Error opening duty file %s\n", strerror(errno));
				exit(EXIT_FAILURE);
			}*/
		}
	}
	sys2 = fopen("/sys/class/gpio/export", "w");
	if (sys2 == NULL) {
		printf("Error opening gpio export: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	fseek(sys2, 0, SEEK_SET);
	// Writes the value corresponding to the GPIO digital pins used
	fprintf(sys2, "%d", SER_DATA_);
	fflush(sys2);
	fprintf(sys2, "%d", SR_CLOCK_);
	fflush(sys2);
	fprintf(sys2, "%d", LATCH_);
	fflush(sys2);
	fprintf(sys2, "%d", RX_);
	fflush(sys2);
	fprintf(sys2, "%d", TX_);
	fflush(sys2);

	// Sets GPIO directions
	SER_dir = fopen("/sys/class/gpio/gpio45/direction", "w");
	if (SER_dir == NULL) {
		printf("Error opening SER_dir file %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	setOut(SER_dir);

	SR_dir = fopen("/sys/class/gpio/gpio66/direction", "w");
	if (SR_dir == NULL) {
		printf("Error opening SR_dir file %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	setOut(SR_dir);

	LATCH_dir = fopen("/sys/class/gpio/gpio69/direction", "w");
	if (LATCH_dir == NULL) {
		printf("Error opening LATCH_dir file %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	setOut(LATCH_dir);

	TX_dir = fopen("/sys/class/gpio/gpio67/direction", "w");
	if (TX_dir == NULL) {
		printf("Error opening TX_dir file %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	setOut(TX_dir);

	RX_dir = fopen("/sys/class/gpio/gpio68/direction", "w");
	if (RX_dir == NULL) {
		printf("Error opening RX_dir file %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	setOut(RX_dir);

	// Sets up value file pointers
	SER_DATA_VAL = fopen("/sys/class/gpio/gpio45/value", "w");
	if (SER_DATA_VAL == NULL) {
		printf("Error opening SER_DATA_VAL file %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	fseek(SER_DATA_VAL, 0, SEEK_SET);

	SR_CLOCK_VAL = fopen("/sys/class/gpio/gpio66/value", "w");
	if (SR_CLOCK_VAL == NULL) {
		printf("Error opening SR_CLOCK_VAL file %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	fseek(SR_CLOCK_VAL, 0, SEEK_SET);

	LATCH_VAL = fopen("/sys/class/gpio/gpio69/value", "w");
	if (LATCH_VAL == NULL) {
		printf("Error opening LATCH_VAL file %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	fseek(LATCH_VAL, 0, SEEK_SET);

	TX_VAL = fopen("/sys/class/gpio/gpio67/value", "w");
	if (LATCH_VAL == NULL) {
		printf("Error opening TX_VAL file %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	fseek(TX_VAL, 0, SEEK_SET);

	RX_VAL = fopen("/sys/class/gpio/gpio68/value", "w");
	if (LATCH_VAL == NULL) {
		printf("Error opening RX_VAL file %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	fseek(RX_VAL, 0, SEEK_SET);

	setup = true;
}

// Closes all file pointers used
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

// Sets given GPIO to output
void setOut(FILE *dir) {
	fseek(dir, 0, SEEK_SET);
	fprintf(dir, "%s", "out");
	fflush(dir);
}

// Changes PWM pin to given duty cycle and period
void changePWM(int duty, int period) {
	if (PWM_DUTY == NULL) {
		printf("PWM_DUTY is NULL somehow\n");
		fflush(stdout);
	} else {
		fprintf(PWM_DUTY, "%d", duty);
		fflush(PWM_DUTY);
	}

	if (PWM_T == NULL) {
		printf("PWM_DUTY is NULL somehow\n");
		fflush(stdout);
	} else {
		fprintf(PWM_T, "%d", period);
		fflush(PWM_T);
	}
}


// Sends command through shift register to command the H-bridge
// Note:
// {A: Standby, B: AIN1 (left motor), C: AIN2 (left motor), D: BIN1 (right motor), E: BIN2 (right motor)} => {0,0,0,E,D,C,B,A}
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
	
	// Load to parallel output
	fprintf(LATCH_VAL, "%d", 1);
	fflush(LATCH_VAL);
	usleep(50);
	fprintf(LATCH_VAL, "%d", 0);
	fflush(LATCH_VAL);

}
