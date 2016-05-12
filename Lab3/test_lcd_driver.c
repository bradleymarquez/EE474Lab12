#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#define NEW_CHAR_DIR "/dev/lcd_driver"

int main(void)
{
	int size_buf = 1;
	int fd;
	char write_buf[100];
	printf("Pre-open\n");
	fd = open(NEW_CHAR_DIR, O_RDWR);
	printf("Post-open\n");
	if (fd < 0)
	{
		printf("File %s cannot be opened\n", NEW_CHAR_DIR);
		exit(1);
	}
	strcpy(write_buf, "Hello this is a test for the LCD");
	write(fd, write_buf, 32);
	usleep(3000000);

	strcpy(write_buf, "Hello this is a test for the LCD blah");
	write(fd, write_buf, 37);
	usleep(3000000);

	strcpy(write_buf, "Hello");
	write(fd, write_buf, 5);
	usleep(3000000);

	strcpy(write_buf, "");
	write(fd, write_buf, 0);
	usleep(3000000);

	strcpy(write_buf, "testtesttesttesttesttesttesttesttesttesttesttest");
	printf("Pre-write\n");
	write(fd, write_buf, 48);
	printf("Post-write\n");
	usleep(3000000);
	//read(fd, read_buf, sizeof(read_buf));
	//printf("Successfully read from device file\n");
	//printf("The words are: %s\n", read_buf);

	close(fd);
	return 0;
}
