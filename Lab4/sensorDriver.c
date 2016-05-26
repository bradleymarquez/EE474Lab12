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

#define FRONT 0
#define BACK 1
#define LEFT 2
#define RIGHT 3
#define NUM_SENSORS 4
#define SAMPLES 20
#define SAMPLE_RATE 800000
#define MIN_DIST 2000

typedef struct sensor_sample_struct {
	int sample_i;
	int sample_space[NUM_SENSORS][SAMPLES];
	int average[NUM_SENSORS];
	char status[4];
	FILE *files[NUM_SENSORS];
} sensor_sample;
sensor_sample sensor;

struct sigaction sa;
struct itimerval timer;
char *path = "/tmp/sensor";
int fifo_fd;

void timer_Init();
void timer_handler(int sig);
void closeDown(int signo);
void closeFiles();

int main(int argc, char **argv) {
	signal(SIGINT, closeDown);
	memset((void *) &sensor, 0, sizeof(sensor_sample));

	if (system("echo cape-bone-iio > /sys/devices/bone_capemgr.9/slots") == -1) {
		printf("Error creating ADC files\n");
		return 1;
	}

	// Setup sensor files
	sensor.files[BACK] = fopen("/sys/bus/iio/devices/iio:device0/in_voltage0_raw", "r");
	sensor.files[FRONT] = fopen("/sys/bus/iio/devices/iio:device0/in_voltage6_raw", "r");
	sensor.files[LEFT] = fopen("/sys/bus/iio/devices/iio:device0/in_voltage2_raw", "r");
	sensor.files[RIGHT] = fopen("/sys/bus/iio/devices/iio:device0/in_voltage4_raw", "r");

	if (sensor.files[FRONT] == NULL ||
	    sensor.files[LEFT] == NULL ||
	    sensor.files[RIGHT] == NULL ||
	    sensor.files[BACK] == NULL) {
		printf("Error opening ADC pins %s\n", strerror(errno));
		return 1;
	}

	// Create pipe file
	if (access(path, F_OK) != 0) {
		if (mkfifo(path, 0666) == -1) {
			printf("Error creating pipe: %s\n", strerror(errno));
			closeFiles();
			printf("Exit close files\n");
			return 1;
		}
	}

	// Open pipe
	fifo_fd = open(path, O_RDWR);
	if (fifo_fd == -1) {
		printf("Error opening pipe: %s\n", strerror(errno));
		closeFiles();
		return 1;
	}
	timer_Init();
	while(1);
}

// Timer interupt setup
void timer_Init() {
	memset (&sa, 0, sizeof (sa));
	sa.sa_handler = &timer_handler;
	sigaction (SIGVTALRM, &sa, NULL);
	
	// Configure the timer to expire after SAMPLE_RATE msec...
	timer.it_value.tv_sec = 0;
	timer.it_value.tv_usec = SAMPLE_RATE;
	// ... and every SAMPLE_RATE msec after that.
	timer.it_interval.tv_sec = 0;
	timer.it_interval.tv_usec = SAMPLE_RATE;
	// Start a virtual timer. It counts down whenever this process is
	// executing.
	setitimer (ITIMER_VIRTUAL, &timer, NULL);
}

// Timer interrupt handler
void timer_handler(int sig) {
	int i;
	char oldStatus[4];
	for (i = 0; i < NUM_SENSORS; i++) {
		int pinValue;
		fseek(sensor.files[i], 0, SEEK_SET);
		fscanf(sensor.files[i], "%d", &pinValue);
		printf("i: %d, pinValue: %d\n", i, pinValue);
		fflush(stdout);		
		sensor.average[i] -= sensor.sample_space[i][sensor.sample_i % SAMPLES] / SAMPLES;
		sensor.average[i] += pinValue / SAMPLES;
		sensor.sample_space[i][sensor.sample_i % SAMPLES] = pinValue;
		oldStatus[i] = sensor.status[i];
		if (sensor.average[i] < MIN_DIST) {
			sensor.status[i] = '1';
		} else {
			sensor.status[i] = '0';
		}
	}
	if (oldStatus[0] != sensor.status[0] ||
	    oldStatus[1] != sensor.status[1] ||
	    oldStatus[2] != sensor.status[2] ||
	    oldStatus[3] != sensor.status[3]) {
		// Wirte the sensor.status to a named pipe
		write(fifo_fd, sensor.status, NUM_SENSORS);

		// Send signal to master program that we have written to the named pipe
		// which means the sensor status values have changed
		if (system("pkill --signal SIGUSR1 sensorDriverTest") == -1) {
			printf("Error sending SIGUSR1\n");
		}
	}
	sensor.sample_i++;
}

void closeDown(int signo) {
	unlink(path);
	closeFiles();
	exit(EXIT_SUCCESS);
}

void closeFiles() {
	int i;
	for (i = 0; i < NUM_SENSORS; i++) {
		fclose(sensor.files[i]);
	}
}

