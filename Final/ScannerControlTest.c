#include "ScannerControl.h"

void sigHandler(int signo) {
	if (signo == SIGINT) {
		closeScanner();
		exit(EXIT_SUCCESS);
	}
}

int main() {
	servoInit();
	signal(SIGINT, sigHandler);
	while(1) {
		scan_data myData;
		printf("Press enter to do a scan");
		char angle[1024];
		scanf("%s", angle);
		scan(10, 1, &myData);
		printNums(&myData);
	}
	closeScanner();
}
