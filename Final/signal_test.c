#include <signal.h>
#include <stdio.h>

void handler(int signo) {
	if (signo == SIGUSR1) {
		printf("Got SIGUSR1 signal\n");
	}
}

int main() {
	struct test {
		int integer;
	} myt;
	signal(SIGUSR1, handler);
	while(1);
}

