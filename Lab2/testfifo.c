#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

void main() {
  // Create pipe
  int fd;
  char *path = "./testfifo";
  if (access(path, F_OK) != 0) {
    printf("Creating pipe \"testfifo\"\n");
    mkfifo(path, 0666);
  }

  // Open pipe
  printf("Opening pipe\n");
  fd = open(path, O_RDWR);
  if (fd == -1) {
    printf("Error open: %s\n", strerror(errno));
    return;
  }
  // Write to pipe
  printf("Writing message to pipe\n");
  ssize_t bytes, written = 0;
  char *send = "Did you get my message?\n";
  while (written < strlen(send)) {
    bytes = write(fd, send, strlen(send) - written);
    if (bytes == -1) {
      if (errno != EINTR) {
        printf("Error on write: %s\n", strerror(errno));
        return;
      }
      continue;
    }
    written += bytes;
  } 

  // Read from pipe
  printf("Reading message from pipe\n");
  ssize_t bytesread = 0;
  bytes = 0;
  char message[strlen(send) + 1];
  while (bytesread < strlen(send)) {
    bytes = read(fd, message, strlen(send) - bytesread);
    if (bytes == -1) {
      if (errno != EINTR) {
        printf("Error on read: %s\n", strerror(errno));
        return;
      }
      continue;
    }
    bytesread += bytes;
  }
  message[strlen(send)] = '\0';

  printf("Message is: %s", message);
  return;
}
