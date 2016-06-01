#include <stdlib.h>
#include <stdio.h>

#define FILENAME "signal_test"

int main() {
	if (system("pkill --signal SIGUSR1 %s", FILENAME) == -1) {
		printf("Error\n");
	} else {
		printf("Send sig\n");
	}
}

