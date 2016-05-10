#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#define NEW_CHAR_DIR "/dev/lcd_driver"

int main(void)
{
	int size_buf = 1;
	int fd;
	char write_buf[40];
	fd = open(NEW_CHAR_DIR, O_RDWR);
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
	printf("Successfully wrote to device file\n");

	//read(fd, read_buf, sizeof(read_buf));
	//printf("Successfully read from device file\n");
	//printf("The words are: %s\n", read_buf);

	close(fd);
	return 0;
}
