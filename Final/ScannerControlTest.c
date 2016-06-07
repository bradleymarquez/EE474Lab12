#include "ScannerControl.h"

void sigHandler(int signo) {
	if (signo == SIGINT) {
		closeScanner();
		exit(EXIT_SUCCESS);
	}
}

int main() {
	scannerInit();
	printf("Exit scannerInit\n");
	signal(SIGINT, sigHandler);
	while(1) {
		scan_data myData;
		printf("Press enter to do a scan");
		char angle[1024];
		printf("here1\n");
		scanf("%s", angle);
		printf("here2\n");
		scan(300, 1, &myData);
		printf("here3\n");
		printNums(&myData);
		printf("here4\n");
		printDisplay(&myData);
		printf("here5\n");
	}
	closeScanner();
}
