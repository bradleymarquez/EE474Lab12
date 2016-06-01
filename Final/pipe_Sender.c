#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

int main()
{
    int fd;
    char * sensor_fifo = "/tmp/my_fifo";
    char * error_Msg = "Error opening file, checking if file exists.\n";
    /* create the FIFO (named pipe) */

    /* write "Hi" to the FIFO */
    if( (fd = open(sensor_fifo, O_WRONLY)) <0){
        //write(2, error_Msg, sizeof(error_Msg));
        printf("%s", error_Msg);
        return -1; //return error code 
    }
    write(fd, "Greeting from the sender!", sizeof("Greeting from the sender!"));
    close(fd);

    /* remove the FIFO */
    //unlink(sensor_fifo);
    return 0;
}