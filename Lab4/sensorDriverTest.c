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

char *path = "/tmp/sensor";

void usrHandler(int signo);
void closeHandler(int signo);

int main(int argc, char **argv) {
	signal(SIGUSR1, usrHandler);
	signal(SIGINT, closeHandler);
	while(1);	
}

void usrHandler(int signo) {
	printf("Received signal\n");
	// Open pipe
	int fifo_fd = open(path, O_RDWR);
	if (fifo_fd == -1) {
		printf("Error opening pipe: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	char sensors[5];
	sensors[4] = '\0';
	read(fifo_fd, sensors, 4);
	printf("Sensor reading: %s\n", sensors);
	close(fifo_fd);
}

void closeHandler(int signo) {
	exit(EXIT_SUCCESS);
}
