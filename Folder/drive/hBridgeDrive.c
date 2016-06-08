/*	hBridgeDrive.c
 * Brad Marquez, Joseph Rothlin, Aigerim Shintemirova
 * 24 / May / 2016
 *
 * Takes in inputs from the Chrome NXT Bluetooth Controller
 * to control the Robotank
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
#include "arduino-serial-lib.c"

#define SER_DATA_ 112 // Serial Data (Shift Register) - GPIO_PIN_45
#define SR_CLOCK_ 49 // SR Clock (Shift Register) - GPIO_PIN_66
#define LATCH_ 115 // Latch (Shift Register) - GPIO_PIN_69
#define RX_ 68 // RX of Beaglebone - GPIO_PIN_68
#define TX_ 67 // TX of Beaglebone - GPIO_PIN_67
#define FRONT 0 // Front Sensor Index
#define BACK 1 // Back Sensor Index
#define LEFT 2 // Left Sensor Index
#define RIGHT 3 // Right Sensor Index
#define NUM_SENSORS 4 // Sensor Array Size
#define PWM_PERIOD 10000000 // in nanoseconds
#define READ_RATE 500

#define BAUDRATE 115200 // bpsec
#define BUF_MAX 256
#define TIMEOUT 100

//struct termios termAttr; 
struct sigaction sa;
struct itimerval timer;

static FILE *sys, *sys2, *PWM_T, *PWM_DUTY, *SER_DATA_VAL, *SR_CLOCK_VAL, *LATCH_VAL, *SER_dir, *SR_dir, *LATCH_dir;
static int fd_RXTX = -1;
char *RXTX_path = "/dev/ttyO1";
bool setup = false;

void pointSetup(void);
void closePointers(void);
void setOut(FILE*);
void changePWM(int, int);
void command(unsigned char);
void closeHandler(int);
void goForward(void);
void goLeft(void);
void goRight(void);
void goBackward(void);
void goStop(void);
void configureBluetooth(void);
void bluetoothSend(char*);
void timer_read_Init(void);
void control_handler(int);
int main(){
	signal(SIGINT, closeHandler); // Ctrl+C Interrupt
	pointSetup(); // Sets up Pointers
	//configureBluetooth();
	timer_read_Init();
	while (1){
	}
	return 0;
}

// Timer interupt setup
void timer_read_Init() {
	memset (&sa, 0, sizeof (sa));
	sa.sa_handler = &control_handler;
	sigaction (SIGVTALRM, &sa, NULL);
	
	// Configure the timer to expire after SAMPLE_RATE msec...
	timer.it_value.tv_sec = 0;
	timer.it_value.tv_usec = READ_RATE;

	// ... and every SAMPLE_RATE msec after that.
	timer.it_interval.tv_sec = 0;
	timer.it_interval.tv_usec = READ_RATE;

	// Start a virtual timer. It counts down whenever this process is
	// executing.
	setitimer (ITIMER_VIRTUAL, &timer, NULL);
}

// Timer interrupt handler
void control_handler(int sig) {
	unsigned char buf[BUF_MAX];
	serialport_read(fd_RXTX, buf, BUF_MAX, TIMEOUT);
	//printf("Received data: %c\n",*buf);

	//printf("0x%x\n", buf[5]);
	if (buf[4] == '\x0') {
		if (buf[5] == '\x0') {
			printf("STOP\n");
			fflush(stdout);
			goStop();
		} else if (buf[5] == '\x64') {
			printf("A = BACKWARD\n");
			fflush(stdout);
			goBackward();
		} else if (buf[5] == '\x9C') {
			printf("Q = FORWARD\n");
			fflush(stdout);
			goForward();
		}
	} else if (buf[4] == '\x1') {
		if (buf[5] == '\x0') {
			printf("STOP\n");
			fflush(stdout);
			goStop();
		} else if (buf[5] == '\x64') {
			printf("S = LEFT\n");
			fflush(stdout);
			goLeft();
		} else if (buf[5] == '\x9C') {
			printf("W = RIGHT\n");
			fflush(stdout);
			goRight();
		}
	}
}

// Configures the bluetooth, setting name to Workstation12, wakes up
// and reboots
void configureBluetooth() {
	bluetoothSend("$$$");
	bluetoothSend("SN,Workstation12\n");
	bluetoothSend("W\n");
	bluetoothSend("T,1\n");
	bluetoothSend("GF\n");
	bluetoothSend("SO,%\n");
	bluetoothSend("SU,115200\n");
	bluetoothSend("GK\n");
	bluetoothSend("R,1\n");
	bluetoothSend("---\n");
}

// Sends data to the bluetooth and prints out received Data
void bluetoothSend(char *buf) {
	char buf2[BUF_MAX];
	serialport_flush(fd_RXTX);
	int writing = serialport_write(fd_RXTX, buf);
	if (writing == -1) {
		printf("Error on write... : ");
		fflush(stdout);
	} else {
		printf("Write success... : ");
		printf("%s\n", buf);
		fflush(stdout);
	}
	serialport_read_until(fd_RXTX, buf2, '\n', BUF_MAX, TIMEOUT);
	printf("Received data: %s\n",buf2);
	fflush(stdout);
}
// Commands the H-Bridge to drive both motors CW
void goForward() {
	command(0x15);
	changePWM(0, PWM_PERIOD);
}

// Commands the H-bridge to drive both motors CCW
void goBackward() {
	command(0xB);
	changePWM(0, PWM_PERIOD);
}

// Commands the H-bridge to drive the "left motor" CCW
// and the "right motor" CW
void goLeft() {
	command(0xD);
	changePWM(0, PWM_PERIOD);
}

// Commands the H-bridge to drive the "right motor" CCW
// and the "left motor" CW
void goRight() {
	command(0x13);
	changePWM(0, PWM_PERIOD);
}

// Commands the H-bridge to resist motion on both motors
void goStop() {
	command(0x01);
	changePWM(PWM_PERIOD, PWM_PERIOD);
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

// Sets up access to the PWM pin and GPIO pins
void pointSetup(){
	sys = fopen("/sys/devices/bone_capemgr.9/slots", "w");
	if (sys == NULL) {
		printf("Error opening sys file: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	fseek(sys, 0, SEEK_SET);
	
	// Create UART file
	fprintf(sys, "BB-UART1");
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

	// Sets GPIO directions
	SER_dir = fopen("/sys/class/gpio/gpio112/direction", "w");
	if (SER_dir == NULL) {
		printf("Error opening SER_dir file %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	setOut(SER_dir);

	SR_dir = fopen("/sys/class/gpio/gpio49/direction", "w");
	if (SR_dir == NULL) {
		printf("Error opening SR_dir file %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	setOut(SR_dir);

	LATCH_dir = fopen("/sys/class/gpio/gpio115/direction", "w");
	if (LATCH_dir == NULL) {
		printf("Error opening LATCH_dir file %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	setOut(LATCH_dir);

	// Sets up value file pointers
	SER_DATA_VAL = fopen("/sys/class/gpio/gpio112/value", "w");
	if (SER_DATA_VAL == NULL) {
		printf("Error opening SER_DATA_VAL file %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	fseek(SER_DATA_VAL, 0, SEEK_SET);

	SR_CLOCK_VAL = fopen("/sys/class/gpio/gpio49/value", "w");
	if (SR_CLOCK_VAL == NULL) {
		printf("Error opening SR_CLOCK_VAL file %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	fseek(SR_CLOCK_VAL, 0, SEEK_SET);

	LATCH_VAL = fopen("/sys/class/gpio/gpio115/value", "w");
	if (LATCH_VAL == NULL) {
		printf("Error opening LATCH_VAL file %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	fseek(LATCH_VAL, 0, SEEK_SET);
	
	fd_RXTX = serialport_init(RXTX_path, BAUDRATE);
	printf("fd_RXTX = %i\n", fd_RXTX);
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
		printf("PWM_T is NULL somehow\n");
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


