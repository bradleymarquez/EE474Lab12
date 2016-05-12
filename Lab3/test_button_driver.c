#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#define NEW_CHAR_DIR "/dev/button_driver"
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3
#define PRESS 4
#define NUM_BUTTONS 5

int main(void)
{
	int press = 0;
	int fd;
	int write_buf[NUM_BUTTONS];
	printf("Pre-open\n");
	fd = open(NEW_CHAR_DIR, O_RDWR);
	printf("Post-open\n");
	if (fd < 0)
	{
		printf("File %s cannot be opened\n", NEW_CHAR_DIR);
		exit(1);
	}
	printf("Open successful\n");
	while (!press) {
		//printf("Attempting to read\n");
		//printf("%d\n", NUM_BUTTONS * sizeof(int));
		read(fd, write_buf, NUM_BUTTONS * sizeof(int));
		//printf("Exited from read\n");
		int i;
		for (i = 0; i < NUM_BUTTONS; i++) {
			printf("%d ", write_buf[i]);
		}
		printf("\n");
		if (write_buf[PRESS]) {
			press = 1;
			printf("PRESS\n");
		} else if (write_buf[UP]) {
			printf("UP\n");
		} else if (write_buf[DOWN]) {
			printf("DOWN\n");
		} else if (write_buf[LEFT]) {
			printf("LEFT\n");
		} else if (write_buf[RIGHT]) {
			printf("RIGHT\n");
		}
	}

	close(fd);
	return 0;
}
