#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h> //time interrupt librarires
#include <time.h> // time interrupt libraries
#include <stdio.h>

#define CLOCKID CLOCK_REALTIME

#define SIG SIGRTMIN

#define GPIO_PIN_PWMA 	//
#define GPIO_PIN_AIN2 	//
#define GPIO_PIN_AIN1 	//
#define GPIO_PIN_STBY 	//
#define GPIO_PIN_BIN1 	//
#define GPIO_PIN_BIN2 	//
#define GPIO_PIN_PWMB 	//

#define errExit(msg)    do { perror(msg); exit(EXIT_FAILURE); \
                               } while (0)
/*static void print_siginfo(siginfo_t *si)
       {
           timer_t *tidp;
           int or;

           tidp = si->si_value.sival_ptr;

           printf("    sival_ptr = %p; ", si->si_value.sival_ptr);
           printf("    *sival_ptr = 0x%lx\n", (long) *tidp);

           or = timer_getoverrun(*tidp);
           if (or == -1)
               errExit("timer_getoverrun");
           else
               printf("    overrun count = %d\n", or);
       }*/

static void handler(int sig, siginfo_t *si, void *uc)
       {
           /* Note: calling printf() from a signal handler is not
              strictly correct, since printf() is not async-signal-safe;
              see signal(7) */

            //printf("Caught signal %d\n", sig);
           //print_siginfo(si);
           //signal(sig, SIG_IGN);

        FILE *ain, *avalFront, *avalBack, *avalLeft, *avalRight;
		int valFront, valBack, valLeft, valRight;

		avalFront = fopen("/sys/bus/iio/devices/iio:device0/in_voltage3_raw", "r");	//front
		fseek(avalFront,0,SEEK_SET);
		fscanf(avalFront,"%d",&valFront);
		fclose(avalFront);

		usleep(3);

		/*avalLeft = fopen("/sys/bus/iio/devices/iio:device0/in_voltage1_raw", "r");	//left
		fseek(avalLeft,0,SEEK_SET);
		fscanf(avalLeft,"%d",&valLeft);
		fclose(avalLeft);*/
       }

int main(int argc, char *argv[])
       {
           timer_t timerid;
           struct sigevent sev;
           struct itimerspec its;
           long long freq_nanosecs;
           sigset_t mask;
           struct sigaction sa;

           if (argc != 3) {
               fprintf(stderr, "Usage: %s <sleep-secs> <freq-nanosecs>\n",
                       argv[0]);
               exit(EXIT_FAILURE);
           }

           /* Establish handler for timer signal */

           printf("Establishing handler for signal %d\n", SIG);
           sa.sa_flags = SA_SIGINFO;
           sa.sa_sigaction = handler;
           sigemptyset(&sa.sa_mask);
           if (sigaction(SIG, &sa, NULL) == -1)
               errExit("sigaction");

           /* Block timer signal temporarily */

           printf("Blocking signal %d\n", SIG);
           sigemptyset(&mask);
           sigaddset(&mask, SIG);
           if (sigprocmask(SIG_SETMASK, &mask, NULL) == -1)
               errExit("sigprocmask");

           /* Create the timer */

           sev.sigev_notify = SIGEV_SIGNAL;
           sev.sigev_signo = SIG;
           sev.sigev_value.sival_ptr = &timerid;
           if (timer_create(CLOCKID, &sev, &timerid) == -1)
               errExit("timer_create");

           printf("timer ID is 0x%lx\n", (long) timerid);

           /* Start the timer */

           freq_nanosecs = atoll(argv[2]);
           its.it_value.tv_sec = freq_nanosecs / 1000000000;
           its.it_value.tv_nsec = freq_nanosecs % 1000000000;
           its.it_interval.tv_sec = its.it_value.tv_sec;
           its.it_interval.tv_nsec = its.it_value.tv_nsec;

           if (timer_settime(timerid, 0, &its, NULL) == -1)
                errExit("timer_settime");

           /* Sleep for a while; meanwhile, the timer may expire
              multiple times */

           printf("Sleeping for %d seconds\n", atoi(argv[1]));
           sleep(atoi(argv[1]));

           /* Unlock the timer signal, so that timer notification
              can be delivered */

           printf("Unblocking signal %d\n", SIG);
           if (sigprocmask(SIG_UNBLOCK, &mask, NULL) == -1)
               errExit("sigprocmask");

           exit(EXIT_SUCCESS);
       }
