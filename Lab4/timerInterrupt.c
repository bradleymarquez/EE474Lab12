/* timerInterrupt.c
 * Brad Marquez, Joseph Rothlin, Aigerim Shintemirova
 * 24 / May / 2016
 *
 *	TODO:
 *  - Send signal to master program
 *  - Figure out how timer interrupts work in general
 *
 */

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

FILE *avalFront, *avalBack, *avalLeft, *avalRight;
int valFront, valBack, valLeft, valRight, counter, doneWaiting,
    valuesOfFront, valuesOfLeft, valuesOfBack, valuesOfRight;

static void handler(int sig, siginfo_t *si, void *uc)
{
    avalFront = fopen("/sys/bus/iio/devices/iio:device0/in_voltage0_raw", "r"); // front
	avalLeft = fopen("/sys/bus/iio/devices/iio:device0/in_voltage6_raw", "r"); // left
	avalRight = fopen("/sys/bus/iio/devices/iio:device0/in_voltage2_raw", "r"); // right
	avalBack = fopen("/sys/bus/iio/devices/iio:device0/in_voltage4_raw", "r"); // back


	/*int tempFront, tempBack, tempLeft, tempRight;
	fseek(avalFront,0,SEEK_SET);
	fseek(avalBack,0,SEEK_SET);
	fseek(avalLeft,0,SEEK_SET);
	fseek(avalRight,0,SEEK_SET);
	fscanf(avalFront,"%d",&tempFront);
	fscanf(avalBack,"%d",&tempBack);
	fscanf(avalLeft,"%d",&tempLeft);
	fscanf(avalRight,"%d",&tempRight);

	counter = (counter + 1) % 10; // check every 10 timer interrupts

	if (counter != 0) {
		valFront = (valFront + tempFront) / counter;
		valBack = (valBack + tempBack) / counter;
		valLeft = (valLeft + tempLeft) / counter;
		valRight = (valRight + tempRight) / counter;
	}*/

        valuesOfFront += valFront;
        valuesOfLeft += valLeft;
		valuesOfRight += valRight;
		valuesOfBack += valBack;

		counter = counter + 1;

		//renew signal
		signal(sig, handler);
		doneWaiting = 1;
}

int main(int argc, char *argv[]) {

    FILE *sys, *dir, *PWMA, *AIN2, *AIN1, *STBY, *BIN1, *BIN2, *PWMB;

	//analog reading setup
	FILE *ain,*aval,*aval1;
	int value,value1,i, j;

	ain = fopen("/sys/devices/bone_capemgr.9/slots", "w");
	fseek(ain,0,SEEK_SET);
	fprintf(ain,"cape-bone-iio");
	fflush(ain);

	//motor setup
	sys = fopen("/sys/class/gpio/export", "w");
	fseek(sys, 0, SEEK_SET);

	fprintf(sys, "%d", GPIO_PIN_PWMA);
	fflush(sys);

	fprintf(sys, "%d", GPIO_PIN_AIN2);
	fflush(sys);

	fprintf(sys, "%d", GPIO_PIN_AIN1);
	fflush(sys);

	fprintf(sys, "%d", GPIO_PIN_STBY);
	fflush(sys);

	fprintf(sys, "%d", GPIO_PIN_BIN1);
	fflush(sys);

	fprintf(sys, "%d", GPIO_PIN_BIN2);
	fflush(sys);

	fprintf(sys, "%d", GPIO_PIN_PWMB);
	fflush(sys);

	dir = fopen("/sys/class/gpio/gpio1/direction", "w");
	fseek(dir, 0, SEEK_SET);
	fprintf(dir, "%s", "out");
	fflush(dir);

	dir = fopen("/sys/class/gpio/gpio2/direction", "w");
	fseek(dir, 0, SEEK_SET);
	fprintf(dir, "%s", "out");
	fflush(dir);

	dir = fopen("/sys/class/gpio/gpio3/direction", "w");
	fseek(dir, 0, SEEK_SET);
	fprintf(dir, "%s", "out");
	fflush(dir);

	dir = fopen("/sys/class/gpio/gpio4/direction", "w");
	fseek(dir, 0, SEEK_SET);
	fprintf(dir, "%s", "out");
	fflush(dir);

	dir = fopen("/sys/class/gpio/gpio5/direction", "w");
	fseek(dir, 0, SEEK_SET);
	fprintf(dir, "%s", "out");
	fflush(dir);

	dir = fopen("/sys/class/gpio/gpio6/direction", "w");
	fseek(dir, 0, SEEK_SET);
	fprintf(dir, "%s", "out");
	fflush(dir);

	dir = fopen("/sys/class/gpio/gpio7/direction", "w");
	fseek(dir, 0, SEEK_SET);
	fprintf(dir, "%s", "out");
	fflush(dir);

	PWMA = fopen("/sys/class/gpio/gpio1/value", "w");
	fseek(PWMA, 0, SEEK_SET);

	AIN2 = fopen("/sys/class/gpio/gpio2/value", "w");
	fseek(AIN2, 0, SEEK_SET);

	AIN1 = fopen("/sys/class/gpio/gpio3/value", "w");
	fseek(AIN1, 0, SEEK_SET);

	STBY = fopen("/sys/class/gpio/gpio4/value", "w");
	fseek(STBY, 0, SEEK_SET);

	BIN1 = fopen("/sys/class/gpio/gpio5/value", "w");
	fseek(BIN1, 0, SEEK_SET);

	BIN2 = fopen("/sys/class/gpio/gpio6/value", "w");
	fseek(BIN2, 0, SEEK_SET);

	PWMB = fopen("/sys/class/gpio/gpio7/value", "w");
	fseek(PWMB, 0, SEEK_SET);

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

    // while loop for time interrupt/enable signal
    while(1)
	{
		doneWaiting = 0; //waiting for the enable
		for(i = 0; i<90000000; i++);	//go off ~three times a second

		valFront = valuesOfFront/counter;
		valLeft = valuesOfLeft/counter;
		valRight = valuesOfRight/counter;
		valBack = valuesOfBack/counter;

		printf("frontValue: %d\n",valFront);
		printf("leftValue: %d\n",valLeft);
		printf("rightValue: %d\n",valRight);
		printf("backValue: %d\n",valBack);
		printf("timesExpired: %d\n",counter);

		counter = 0;
		valuesOfFront = 0;
		valuesOfLeft = 0;
		valuesOfRight = 0;
		valuesOfBack = 0;

		/*if (frontValue > 3200) {	//front sensor is close, so back up

			fprintf(PWMA, "%d", 1);
			fflush(PWMA);
			fprintf(STBY, "%d", 1);
			fflush(STBY);

			fprintf(PWMB, "%d", 1);
			fflush(PWMB);

			fprintf(AIN1, "%d", 0);
			fflush(AIN1);
			fprintf(AIN2, "%d", 1);
			fflush(AIN2);

			fprintf(BIN1, "%d", 1);
			fflush(BIN1);
			fprintf(BIN2, "%d", 0);
			fflush(BIN2);

		} else if (leftValue > 3200) {	//left sensor is close, so turn right

			fprintf(PWMA, "%d", 1);
			fflush(PWMA);
			fprintf(STBY, "%d", 1);
			fflush(STBY);

			fprintf(PWMB, "%d", 1);
			fflush(PWMB);

			fprintf(AIN1, "%d", 1);
			fflush(AIN1);
			fprintf(AIN2, "%d", 0);
			fflush(AIN2);

			fprintf(BIN1, "%d", 1);
			fflush(BIN1);
			fprintf(BIN2, "%d", 0);
			fflush(BIN2);


		} else if (rightValue > 3200) {	//left sensor is close, so turn right

			fprintf(PWMA, "%d", 1);
			fflush(PWMA);
			fprintf(STBY, "%d", 1);
			fflush(STBY);

			fprintf(PWMB, "%d", 1);
			fflush(PWMB);


			fprintf(AIN1, "%d", 0);
			fflush(AIN1);
			fprintf(AIN2, "%d", 1);
			fflush(AIN2);

			fprintf(BIN1, "%d", 0);
			fflush(BIN1);
			fprintf(BIN2, "%d", 1);
			fflush(BIN2);

		} else {	//go forward
			fprintf(AIN1, "%d", 1);
			fflush(AIN1);
			fprintf(AIN2, "%d", 0);
			fflush(AIN2);
			fprintf(PWMA, "%d", 1);
			fflush(PWMA);
			fprintf(STBY, "%d", 1);
			fflush(STBY);

			fprintf(BIN1, "%d", 0);
			fflush(BIN1);
			fprintf(BIN2, "%d", 1);
			fflush(BIN2);
			fprintf(PWMB, "%d", 1);
			fflush(PWMB);
		}

	}
*/
    exit(EXIT_SUCCESS);
	fclose(avalFront);
	fclose(avalLeft);
	fclose(avalRight);
	fclose(avalBack);
       }
       }
