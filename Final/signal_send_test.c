#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#define SIG_USER 20




struct sigaction sa;
struct itimerval timer;
int pid;
void timer_Init();

//timer interrupt handler
void timer_handler(int whatever)
{
  /*send user defined signal (SIG_USER) to the process with PID
    kill() doesn't neccessarily mean to terminate another process,
    and in this case, it means to send signal "SIG_USER" to the
    process with "pid"
  */
  if(kill(pid,SIG_USER) != 0){ 
    printf("Can't send msg\n");
    exit(0);
  }
  printf("Signal sent\n");
}

int main(int argc, char *argv[])
{	
   int count;

  printf ("This program was called with \"%s\".\n",argv[0]);

  if (argc > 1)
  {
    for (count = 1; count < argc; count++)
    {
      printf("argv[%d] = %s\n", count, argv[count]);
    }
  }else{
    printf("The command needs more arguments.\n"); 
    return -1; 
  }

  pid = atoi(argv[1]);
  timer_Init();
  while(1);

	return 0;
}

void timer_Init()
{
   memset (&sa, 0, sizeof (sa));
   sa.sa_handler = &timer_handler;
   sigaction (SIGVTALRM, &sa, NULL);

   /* Configure the timer to expire after 250 msec... */
   timer.it_value.tv_sec = 0;
   timer.it_value.tv_usec = 250000;
   /* ... and every 250 msec after that. */
   timer.it_interval.tv_sec = 0;
   timer.it_interval.tv_usec = 250000;
   /* Start a virtual timer. It counts down whenever this process is
     executing. */
   setitimer (ITIMER_VIRTUAL, &timer, NULL);
}
