//useful url for read()
//http://pubs.opengroup.org/onlinepubs/009695399/functions/read.html

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#define MAX_BUF 1024

int main()
{
    int fd;
    /*
    why is the file in "tmp"? because "tmp" is a directory stores
    temporary files such as the named pipe we are creating here
    */
    char * sensor_fifo = "/tmp/my_fifo"; 
    char buf[MAX_BUF];
    /* create the FIFO (named pipe) */
    mkfifo(sensor_fifo, 0666);

    while(1){ //main loop
        fd = open(sensor_fifo, O_RDONLY);
        read(fd, buf, MAX_BUF); //blocking
        printf("Received: %s\n", buf);
        close(fd);
    }
    
    /* remove the FIFO */
    unlink(sensor_fifo);

    return 0;
}