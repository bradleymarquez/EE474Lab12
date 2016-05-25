#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>

#define FRONT 1
#define BACK 2
#define LEFT 3
#define RIGHT 4
#define NUM_SENSORS 4
#define SAMPLES 20
#define SAMPLE_RATE 5000
#define MIN_DIST 2000

struct sensor_sample {
	int sample_i = 0;
	int sample_space[NUM_SENSORS][SAMPLES];
	int average[NUM_SENSORS] = {0, 0, 0, 0}
	char status[4] = {0, 0, 0, 0};
	FILE *files[NUM_SENSORS];
} sensor;

struct sigaction sa;
struct itimerval timer;
char *path = "/root/sensor";
int fifo_fd;

int main(int argc, char **argv) {
	signal(SIGINT, closeDown);

	// Setup sensor files
	sensor.files[FRONT] = fopen("/sys/bus/iio/devices/iio:device0/in_voltage0_raw", "r");
	sensor.files[LEFT] = fopen("/sys/bus/iio/devices/iio:device0/in_voltage6_raw", "r");
	avalRight[RIGHT] = fopen("/sys/bus/iio/devices/iio:device0/in_voltage2_raw", "r");
	avalBack[BACK] = fopen("/sys/bus/iio/devices/iio:device0/in_voltage4_raw", "r");

	// Create pipe file
	if (access(path, F_OK) == 0) {
		printf("Error Creating named pipe\n");
		return 1;
	}
	mkfifo(path, 0666);

	// Open pipe
	fifo_fd = open(path, O_RDWR);
	if (fifo_fd == -1) {
		printf("Error open: %s\n", strerror(errno));
		return 1;
	}

	memset(sensor.sample_space, 0, NUM_SENSORS * SAMPLES * sizeof(int));
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
		fseek(sensor.files[i], 0, SEEK_SET);
		fscanf(sensor.files[i], "%d", &(sensor.sample_space[i][sensor.sample_i % SAMPLES]));
		sensor.average[i] -= sensor.sample_space[i][sensor.sample_i % SAMPLES] / SAMPLES;
		sensor.average += pinValue / SAMPLES
		sensor.sample_space[i][sensor.sample_i % SAMPLES] = pinValue
		oldStatus[i] = sensor.status[i];
		if (sensor.average[i] < MIN_DIST) {
			sensor.status[i] = 1;
		} else {
			sensor.status[i] = 0;
		}
	}
	if (oldStatus[0] != sensor.status[0] ||
	    oldStatus[1] != sensor.status[1] ||
	    oldStatus[2] != sensor.status[2] ||
	    oldStatus[3] != sensor.status[3]) {
		// Wirte the sensor.status to a named pipe
		fwrite(sensor.status, 1, NUM_SENSORS, fifo_fd);

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
	int i;
	for (i = 0; i < NUM_SENSORS; i++) {
		close(sensor.files[i];
	}
	exit(EXIT_SUCCESS);
}

