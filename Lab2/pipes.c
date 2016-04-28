#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
void display(char*);
int main() {
    char *path = "/tmp/fifo";

    mkfifo(path, 0666);
    display(path);
    return 0;
}

void display(char *path) {
    int fd;
    char buf[512];
    fd = open(path, O_RDWR);
    read(fd, buf, sizeof(buf));
    printf("Test \n", buf);
    close(fd);
}
