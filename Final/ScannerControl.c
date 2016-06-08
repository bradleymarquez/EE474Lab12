#include "ScannerControl.h"

// Initialize devices for the scanner
bool scannerInit() {
	printf("Enter scannerInit\n");
	return servoInit() && sensorInit();
}

// Will slowly rotate motor roughly 5 degrees at a time taking a "samples"
// number of readings from the distance sensors "pause" microseconds apart
// after each slight rotation and average the sensor readings from each angle.
// The data will be returned by modifying the scan_data argument.
void scan(int samples, int pause, scan_data *returnData) {
	printf("here0\n");	
	fflush(stdout);
	angleFromZero(0);
	printf("here1\n");
	fflush(stdout);
	//usleep(500000);
	returnData->distances[FRONT][0] = getAverage(samples, pause, FRONT);
	printf("here1.1\n");
	fflush(stdout);
	returnData->distances[BACK][0] = getAverage(samples, pause, BACK);
	printf("here1.2\n");
	fflush(stdout);
	returnData->distances[LEFT][0] = getAverage(samples, pause, LEFT);
	printf("here1.3\n");
	fflush(stdout);
	returnData->distances[RIGHT][0] = getAverage(samples, pause, RIGHT);
	int j;
	printf("here2\n");
	fflush(stdout);
	for (j = 20; j <= 90; j += 10) {
		angleFromZero(j);
		//usleep(500000);
		returnData->distances[FRONT][(j - 10) / 10] = getAverage(samples, pause, FRONT);
		returnData->distances[BACK][(j - 10) / 10] = getAverage(samples, pause, BACK);
		returnData->distances[LEFT][(j - 10) / 10] = getAverage(samples, pause, LEFT);
		returnData->distances[RIGHT][(j - 10) / 10] = getAverage(samples, pause, RIGHT);
	}    
		printf("here3\n"); 
	fflush(stdout);
}

// Reads the specified sensor "samples" number of times "pause" microseconds
// apart and returns the average
int getAverage(int samples, int pause, int whichSens) {
	float sum = 0;
	int i;
	for (i = 0; i < samples; i++) {
		sum += readSens(whichSens);
		usleep(pause);
	}
	int average = sum / samples;
	printf("Average: %i\n", average);
	return average;
}

// Prints the values within the given scan_data to the terminal
void printNums(scan_data *data) {
	int i;
	printf("FRONT\tBACK\tLEFT\tRIGHT\n");
	for (i = 0; i < NUM_OF_ANGLES; i++) {
		printf("%d\t%d\t%d\t%d\n",
		       data->distances[FRONT][i],
		       data->distances[BACK][i],
		       data->distances[LEFT][i],
		       data->distances[RIGHT][i]);
	}
	printf("\n");
}

// Prints a visual representation of the given scan_data to the terminal
void printDisplay(scan_data *data) {
	char display[49][93] =
	{" _________________________________________________________________________________________ \n\0",
	"|                                                                                         |\n\0",
	"|                                                                                         |\n\0",
	"|                                                                                         |\n\0",
	"|                                                                                         |\n\0",
	"|                                                                                         |\n\0",
	"|                                                                                         |\n\0",
	"|                                                                                         |\n\0",
	"|                                                                                         |\n\0",
	"|                                                                                         |\n\0",
	"|                                                                                         |\n\0",
	"|                                                                                         |\n\0",
	"|                                                                                         |\n\0",
	"|                                                                                         |\n\0",
	"|                                                                                         |\n\0",
	"|                                                                                         |\n\0",
	"|                                                                                         |\n\0",
	"|                                                                                         |\n\0",
	"|                                                                                         |\n\0",
	"|                                                                                         |\n\0",
	"|                                                                                         |\n\0",
	"|                                          _____                                          |\n\0",
	"|                                         |  F  |                                         |\n\0",
	"|                                         |     |                                         |\n\0",
	"|                                         |L   R|                                         |\n\0",
	"|                                         |     |                                         |\n\0",
	"|                                         |__B__|                                         |\n\0",
	"|                                                                                         |\n\0",
	"|                                                                                         |\n\0",
	"|                                                                                         |\n\0",
	"|                                                                                         |\n\0",
	"|                                                                                         |\n\0",
	"|                                                                                         |\n\0",
	"|                                                                                         |\n\0",
	"|                                                                                         |\n\0",
	"|                                                                                         |\n\0",
	"|                                                                                         |\n\0",
	"|                                                                                         |\n\0",
	"|                                                                                         |\n\0",
	"|                                                                                         |\n\0",
	"|                                                                                         |\n\0",
	"|                                                                                         |\n\0",
	"|                                                                                         |\n\0",
	"|                                                                                         |\n\0",
	"|                                                                                         |\n\0",
	"|                                                                                         |\n\0",
	"|                                                                                         |\n\0",
	"|                                                                                         |\n\0",
	"|_________________________________________________________________________________________|\n\0"};

	int i;
	for (i = 0; i < 49; i++) {
		printf("%s", display[i]);
	}
	printf("\n\n");

        addObstacles(display, data);

	for (i = 0; i < 49; i++) {
		printf("%s", display[i]);
	}
	return;
}

// Close devices used by the scanner
void closeScanner() {
	closeServo();
	closeSensor();
}

void addObstacles(char display[49][93], scan_data *data) {
	printf("Enter addObstable\n");
	scan_data scaled;
	int i, j, x, y, curr;
	for (i = 0; i < NUM_OF_SENSORS; i++) {
		for (j = 0; j < NUM_OF_ANGLES; j++) {
			scaled.distances[i][j] = (int) (((float)data->distances[i][j] - 1000) / 150);
			if (data->distances[i][j] > 4000) {
				scaled.distances[i][j] = 20;
			} else if (data->distances[i][j] < 1000) {
				scaled.distances[i][j] = 0;
			}
			printf("i: %d, j: %d, scaled Value: %d\n", i, j, scaled.distances[i][j]);
		}
	}
	// Functions for each distance ray starting with straight ahead and moving clockwise
	// For each calculated x and y, the variable that is set to curr +/- # depends on
	// which vaiable is increasing faster. The x variable needs to increase twice as fast
	// as the y because a space is twice as tall as it is wide
	printf("1\n");
	// Start (45, 20) up 1	
	curr = scaled.distances[FRONT][0];
	x = 45;
	y = curr;
	/*printf("x: %d\n", x);
	printf("y: %d\n", y);
	printf("display[x]: %s\n", display[x]);
	printf("display[x][y]: %c\n", display[x][y]);
	printf("1.5\n");
	char temp[92];
	int holder;
	holder = y;
	strcpy(temp, display[y]);
	temp[x] = 'X';
	display[y] = temp;
	printf("2\n");*/
	printf("x: %d\n", x);
	printf("y: %d\n", y);
	/*for (i = 0; i < 49; i++) {
		printf("%s", display[i]);
	}*/
	display[y][x] = 'X';

	// Start (46, 20) up 4 right 1
	curr = scaled.distances[FRONT][1];
	//printf("Holder 2:\t%s\n", display[holder]);
	x = 46 + ((20 - curr) / 4);
	y = curr;
	/*printf("Holder 3:\t%s\n", display[holder]);
	strcpy(temp, display[y]); 
	printf("display[y]:\t%s\ntemp:\t\t%s\nholder:\t%s\n", display[y], temp, display[holder]);	
	temp[x] = 'X'; 
	printf("Holder 4:\t%s\n", display[holder]);
	display[y] = temp;
	printf("display[y]:\t%s\ntemp:\t\t%s\nholder:\t%s\n", display[y], temp, display[holder]);	
	printf("3\n");
	for (i = 0; i < 49; i++) {
		printf("%s", display[i]);
	}*/
	printf("x: %d\n", x);
	printf("y: %d\n", y);
	/*for (i = 0; i < 49; i++) {
		printf("%s", display[i]);
	}*/
	display[y][x] = 'X';

	// Start (47, 20) up 3 right 2
	curr = scaled.distances[FRONT][2];
	x =  47 + ((20 - curr) * 2 / 3);
	y = curr;
	/*display[y][x] = 'X';
	printf("4\n");
	for (i = 0; i < 49; i++) {
		printf("%s", display[i]);
	}*/
	display[y][x] = 'X';
	// Start (48, 20) up 1 right 1
	curr = scaled.distances[FRONT][3];
	x = 48 + (20 - curr);
	y = curr;
	display[y][x] = 'X';
	printf("5\n");
	printf("x: %d\n", x);
	printf("y: %d\n", y);
	/*for (i = 0; i < 49; i++) {
		printf("%s", display[i]);
	}*/
	// Start (49, 20) up 2 right 3
	curr = scaled.distances[FRONT][4];
	x = ((20 - curr) * 2) + 49;
	y = 20 - ((20 - curr) * 3 / 2);
	display[y][x] = 'X';
	printf("6\n");
	printf("x: %d\n", x);
	printf("y: %d\n", y);
	/*for (i = 0; i < 49; i++) {
		printf("%s", display[i]);
	}*/
	// Start (50, 20) up 1 right 2
	curr = scaled.distances[FRONT][5];
	x = ((20 - curr) * 2) + 50;
	y = 20 - ((20 - curr) / 2);
	display[y][x] = 'X';
	printf("7\n");
	printf("x: %d\n", x);
	printf("y: %d\n", y);
	/*for (i = 0; i < 49; i++) {
		printf("%s", display[i]);
	}*/
	// Start (50, 21) up 1 right 3
	curr = scaled.distances[FRONT][6];
	x = ((20 - curr) * 2) + 50;
	y = 21 - ((20 - curr) / 3);
	display[y][x] = 'X';
	printf("8\n");
	printf("x: %d\n", x);
	printf("y: %d\n", y);
	/*for (i = 0; i < 49; i++) {
		printf("%s", display[i]);
	}*/
	// Start (50, 22) up 1 right 4
	curr = scaled.distances[FRONT][7];
	x = ((20 - curr) * 2) + 50;
	y = 22 - ((20 - curr) / 4);
	display[y][x] = 'X';
	printf("9\n");
	printf("x: %d\n", x);
	printf("y: %d\n", y);
	for (i = 0; i < 49; i++) {
		printf("%s", display[i]);
	}
	// Start (50, 23) up 1 right 8
	curr = scaled.distances[FRONT][8];
	x = ((20 - curr) * 2) + 50;
	y = 23 - ((20 - curr) / 8);
	display[y][x] = 'X';
	printf("9.1\n");
	// Start (50, 24) right 1
	curr = scaled.distances[RIGHT][0];
	x = ((20 - curr) * 2) + 50;
	y = 24;
	display[y][x] = 'X';
	printf("9.2\n");
	// Start (50, 25) down 1 right 8
	curr = scaled.distances[RIGHT][1];
	x = ((20 - curr) * 2) + 50;
	y = 25 + ((20 - curr) / 8);
	display[y][x] = 'X';
	printf("9.3\n");
	// Start (50, 26) down 1 right 4
	curr = scaled.distances[RIGHT][2];
	x = ((20 - curr) * 2) + 50;
	y = 26 + ((20 - curr) / 4);
	display[y][x] = 'X';
	printf("9.4\n");
	// Start (50, 27) down 1 right 3
	curr = scaled.distances[RIGHT][3];
	x = ((20 - curr) * 2) + 50;
	y = 27 + ((20 - curr) / 3);
	display[y][x] = 'X';
	printf("10\n"); 

	// Start (50, 28) down 1 right 2
	curr = scaled.distances[RIGHT][4];
	x = ((20 - curr) * 2) + 50;
	y = 28 + ((20 - curr) / 2);
	display[y][x] = 'X';

	// Start (49, 28) down 2 right 3
	curr = scaled.distances[RIGHT][5];
	x = ((20 - curr) * 2) + 49;
	y = 28 + ((20 - curr) * 2 / 3);
	display[y][x] = 'X';

	// Start (48, 28) down 1 right 1
	curr = scaled.distances[RIGHT][6];
	x = 48 + (20 - curr);
	y = (20 - curr) + 28;
	display[y][x] = 'X';

	// Start (47, 28) down 3 right 2
	curr = scaled.distances[RIGHT][7];
	x = 47 + ((20 - curr) * 2 / 3);
	y = (20 - curr) + 28;
	display[y][x] = 'X';

	// Start (46, 28) down 4 right 1
	curr = scaled.distances[RIGHT][8];
	x = 46 + ((20 - curr) / 4);
	y = (20 - curr) + 28;
	display[y][x] = 'X';
	printf("11\n");
	// Start (45, 28) down 1
	curr = scaled.distances[BACK][0];
	x = 45;
	y = (20 - curr) + 28;
	display[y][x] = 'X';

	// Start (44, 28) down 4 left 1
	curr = scaled.distances[BACK][1];
	x = 44 - ((20 - curr) / 4);
	y = (20 - curr) + 28;
	display[y][x] = 'X';

	// Start (43, 28) down 3 left 2
	curr = scaled.distances[BACK][2];
	x = 43 - ((20 - curr) * 2 / 3);
	y = (20 - curr) + 28;
	display[y][x] = 'X';

	// Start (42, 28) down 1 left 1
	curr = scaled.distances[BACK][3];
	x = 42 - (20 - curr) ;
	y = (20 - curr) + 28;
	display[y][x] = 'X';
	printf("12\n");
	// Start (41, 28) down 2 left 3
	curr = scaled.distances[BACK][4];
	x = 41 - ((20 - curr) * 2);
	y = 28 + ((20 - curr) * 2 / 3);
	display[y][x] = 'X';

	// Start (40, 28) down 1 left 2
	curr = scaled.distances[BACK][5];
	x = 40 - ((20 - curr) * 2);
	y = 28 + ((20 - curr) / 2);
	display[y][x] = 'X';

	// Start (40, 27) down 1 left 3
	curr = scaled.distances[BACK][6];
	x = 40 - ((20 - curr) * 2);
	y = 27 + ((20 - curr) / 3);
	display[y][x] = 'X';

	// Start (40, 26) down 1 left 4
	curr = scaled.distances[BACK][7];
	x = 40 - ((20 - curr) * 2);
	y = 26 + ((20 - curr) / 4);
	display[y][x] = 'X';

	// Start (40, 25) down 1 left 8
	curr = scaled.distances[BACK][8];
	x = 40 - ((20 - curr) * 2);
	y = 25 + ((20 - curr) / 8);
	display[y][x] = 'X';

	// Start (40, 24) left 1
	curr = scaled.distances[LEFT][0];
	x = 40 - ((20 - curr) * 2);
	y = 24;
	display[y][x] = 'X';

	// Start (40, 23) up 1 left 8
	curr = scaled.distances[LEFT][1];
	x = 40 - ((20 - curr) * 2);
	y = 23 - ((20 - curr) / 8);
	display[y][x] = 'X';

	// Start (40, 22) up 1 left 4
	curr = scaled.distances[LEFT][2];
	x = 40 - ((20 - curr) * 2);
	y = 22 - ((20 - curr) / 4);
	display[y][x] = 'X';

	// Start (40, 21) up 1 left 3
	curr = scaled.distances[LEFT][3];
	x = 40 - ((20 - curr) * 2);
	y = 21 - ((20 - curr) / 3);
	display[y][x] = 'X';
	printf("13\n");
	// Start (40, 20) up 1 left 2
	curr = scaled.distances[LEFT][4];
	x = 40 - ((20 - curr) * 2);
	y = 20 - ((20 - curr) / 2);
	display[y][x] = 'X';
	printf("14\n");
	// Start (41, 20) up 2 left 3
	curr = scaled.distances[LEFT][5];
	x = 41 - ((20 - curr) * 2);
	y = 20 - ((20 - curr) * 2 / 3);
	display[y][x] = 'X';

	// Start (42, 20) up 1 left 1
	curr = scaled.distances[LEFT][6];
	x = 42 - (20 - curr);
	y = 20 - (20 - curr);
	display[y][x] = 'X';

	// Start (43, 20) up 3 left 2
	curr = scaled.distances[LEFT][7];
	x = 43 - ((20 - curr) * 2 / 3);
	y = 20 - (20 - curr);
	display[y][x] = 'X';

	// Start (44, 20) up 4 left 1
	curr = scaled.distances[LEFT][8];
	x = 44 - ((20 - curr) / 4);
	y = 20 - (20 - curr);
	display[y][x] = 'X';

	printf("Exit addObstable\n");
}

