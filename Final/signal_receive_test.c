#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#define SIG_USER 20

void sig_handler(int signo)
{
    printf("received SIGINT\n");
}

int main(void)
{
  if (signal(SIG_USER, sig_handler) == SIG_ERR)
  	printf("\ncan't catch SIGINT\n");

  while(1) {
    usleep(10000000); //sleep (microsecond) in order to save CPU power
}
  return 0;
}