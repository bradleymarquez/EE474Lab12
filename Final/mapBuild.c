#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define file_Path "putty.txt"
#define NUM_OF_ANGLES 9
#define NUM_OF_SENSORS 4
#define FRONT 0
#define BACK 1
#define LEFT 2
#define RIGHT 3

static time_t oldMTime = 0;
static int modified = 0;
int file_is_modified(const char *path);

typedef struct scan_data_struct {
	int distances[NUM_OF_SENSORS][NUM_OF_ANGLES];
} scan_data;

void closeHandler(int signo);
void printDisplay(scan_data *data);
int file_is_modified(const char *path);
void addObstacles(char display[49][93], scan_data *data);
void print_Data(FILE * infile);
int main() {
	signal(SIGINT, closeHandler); // Ctrl+C Interrupt
    struct stat sb;
	
	if (stat(file_Path, &sb) == -1) {
        perror("stat");
        exit(EXIT_FAILURE);
    }
	
	oldMTime = sb.st_mtime;
	printf("old: %i\n", oldMTime);
	
	FILE *infile;
	infile = fopen(file_Path, "r");       // using relative path name of file
	if (infile == NULL) {
	  perror("fopen");
	  exit(EXIT_FAILURE);
	}
	int i;
	print_Data(infile);
	fflush(infile);
	while (1) {
		if(file_is_modified(file_Path) == 1) {
			print_Data(infile);
			fflush(infile);

		}
	}

    return 0;
}

void print_Data(FILE * infile) {
	//int data [4][9];

	/*fscanf(infile, "%i", data[0][0], data[1][0], data[2][0], data[3][0], data[0][1], data[1][1], data[2][1], data[3][1],
	 data[0][2], data[1][2], data[2][2], data[3][2], data[0][3], data[1][3], data[2][3], data[3][3],
	  data[0][4], data[1][4], data[2][4], data[3][4], data[0][5], data[1][5], data[2][5], data[3][5],
	   data[0][6], data[1][6], data[2][6], data[3][6], data[0][7], data[1][7], data[2][7], data[3][7],
		data[0][8], data[1][8], data[2][8], data[3][8]);*/
	scan_data fixed;
	int i, j, x, y, curr;
	for (j = 0; j < NUM_OF_ANGLES; j++) {
		for (i = 0; i < NUM_OF_SENSORS; i++) {
			int temp = 21;
			while (temp < 0 || temp > 20) {
				//printf("Scanning...");
				temp = fgetc(infile) - 65;
			}
			fixed.distances[i][j] = temp;
			printf("Fixed : %i\n", temp);
		}
	}
	
	printDisplay(&fixed);
}

int file_is_modified(const char *path) {
    struct stat file_stat;
    int err = stat(path, &file_stat);
    if (err != 0) {
        perror(" [file_is_modified] stat");
        exit(errno);
    }
	if (file_stat.st_mtime > oldMTime) {
		printf("old time: %i\n", oldMTime);
		oldMTime = file_stat.st_mtime;
		printf("new time: %i\n", oldMTime);
		return 1;
	}
	return 0;
}

void closeHandler(int signo) {
	if (signo == SIGINT) {
		exit(EXIT_SUCCESS);
	}
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
	/*for (i = 0; i < 49; i++) {
		printf("%s", display[i]);
	}
	printf("\n\n");*/

    addObstacles(display, data);

	for (i = 0; i < 49; i++) {
		printf("%s", display[i]);
	}
	return;
}

void addObstacles(char display[49][93], scan_data *data) {
	printf("Enter addObstable\n");
	int curr, x, y;
	
	// Functions for each distance ray starting with straight ahead and moving clockwise
	// For each calculated x and y, the variable that is set to curr +/- # depends on
	// which vaiable is increasing faster. The x variable needs to increase twice as fast
	// as the y because a space is twice as tall as it is wide
	
	// Start (45, 20) up 1	
	curr = data->distances[FRONT][0];
	x = 45;
	y = curr;
	display[y][x] = 'X';

	// Start (46, 20) up 4 right 1
	curr = data->distances[FRONT][1];
	x = 46 + ((20 - curr) / 4);
	y = curr;
	display[y][x] = 'X';

	// Start (47, 20) up 3 right 2
	curr = data->distances[FRONT][2];
	x =  47 + ((20 - curr) * 2 / 3);
	y = curr;
	display[y][x] = 'X';
	
	// Start (48, 20) up 1 right 1
	curr = data->distances[FRONT][3];
	x = 48 + (20 - curr);
	y = curr;
	display[y][x] = 'X';


	// Start (49, 20) up 2 right 3
	curr = data->distances[FRONT][4];
	x = ((20 - curr) * 2) + 49;
	y = 20 - ((20 - curr) * 3 / 2);
	display[y][x] = 'X';


	// Start (50, 20) up 1 right 2
	curr = data->distances[FRONT][5];
	x = ((20 - curr) * 2) + 50;
	y = 20 - ((20 - curr) / 2);
	display[y][x] = 'X';

	// Start (50, 21) up 1 right 3
	curr = data->distances[FRONT][6];
	x = ((20 - curr) * 2) + 50;
	y = 21 - ((20 - curr) / 3);
	display[y][x] = 'X';

	// Start (50, 22) up 1 right 4
	curr = data->distances[FRONT][7];
	x = ((20 - curr) * 2) + 50;
	y = 22 - ((20 - curr) / 4);
	display[y][x] = 'X';

	// Start (50, 23) up 1 right 8
	curr = data->distances[FRONT][8];
	x = ((20 - curr) * 2) + 50;
	y = 23 - ((20 - curr) / 8);
	display[y][x] = 'X';
	
	// Start (50, 24) right 1
	curr = data->distances[RIGHT][0];
	x = ((20 - curr) * 2) + 50;
	y = 24;
	display[y][x] = 'X';
	
	// Start (50, 25) down 1 right 8
	curr = data->distances[RIGHT][1];
	x = ((20 - curr) * 2) + 50;
	y = 25 + ((20 - curr) / 8);
	display[y][x] = 'X';
	
	// Start (50, 26) down 1 right 4
	curr = data->distances[RIGHT][2];
	x = ((20 - curr) * 2) + 50;
	y = 26 + ((20 - curr) / 4);
	display[y][x] = 'X';
	
	// Start (50, 27) down 1 right 3
	curr = data->distances[RIGHT][3];
	x = ((20 - curr) * 2) + 50;
	y = 27 + ((20 - curr) / 3);
	display[y][x] = 'X';

	// Start (50, 28) down 1 right 2
	curr = data->distances[RIGHT][4];
	x = ((20 - curr) * 2) + 50;
	y = 28 + ((20 - curr) / 2);
	display[y][x] = 'X';

	// Start (49, 28) down 2 right 3
	curr = data->distances[RIGHT][5];
	x = ((20 - curr) * 2) + 49;
	y = 28 + ((20 - curr) * 2 / 3);
	display[y][x] = 'X';

	// Start (48, 28) down 1 right 1
	curr = data->distances[RIGHT][6];
	x = 48 + (20 - curr);
	y = (20 - curr) + 28;
	display[y][x] = 'X';

	// Start (47, 28) down 3 right 2
	curr = data->distances[RIGHT][7];
	x = 47 + ((20 - curr) * 2 / 3);
	y = (20 - curr) + 28;
	display[y][x] = 'X';

	// Start (46, 28) down 4 right 1
	curr = data->distances[RIGHT][8];
	x = 46 + ((20 - curr) / 4);
	y = (20 - curr) + 28;
	display[y][x] = 'X';
	
	// Start (45, 28) down 1
	curr = data->distances[BACK][0];
	x = 45;
	y = (20 - curr) + 28;
	display[y][x] = 'X';

	// Start (44, 28) down 4 left 1
	curr = data->distances[BACK][1];
	x = 44 - ((20 - curr) / 4);
	y = (20 - curr) + 28;
	display[y][x] = 'X';

	// Start (43, 28) down 3 left 2
	curr = data->distances[BACK][2];
	x = 43 - ((20 - curr) * 2 / 3);
	y = (20 - curr) + 28;
	display[y][x] = 'X';

	// Start (42, 28) down 1 left 1
	curr = data->distances[BACK][3];
	x = 42 - (20 - curr) ;
	y = (20 - curr) + 28;
	display[y][x] = 'X';
	
	// Start (41, 28) down 2 left 3
	curr = data->distances[BACK][4];
	x = 41 - ((20 - curr) * 2);
	y = 28 + ((20 - curr) * 2 / 3);
	display[y][x] = 'X';

	// Start (40, 28) down 1 left 2
	curr = data->distances[BACK][5];
	x = 40 - ((20 - curr) * 2);
	y = 28 + ((20 - curr) / 2);
	display[y][x] = 'X';

	// Start (40, 27) down 1 left 3
	curr = data->distances[BACK][6];
	x = 40 - ((20 - curr) * 2);
	y = 27 + ((20 - curr) / 3);
	display[y][x] = 'X';

	// Start (40, 26) down 1 left 4
	curr = data->distances[BACK][7];
	x = 40 - ((20 - curr) * 2);
	y = 26 + ((20 - curr) / 4);
	display[y][x] = 'X';

	// Start (40, 25) down 1 left 8
	curr = data->distances[BACK][8];
	x = 40 - ((20 - curr) * 2);
	y = 25 + ((20 - curr) / 8);
	display[y][x] = 'X';

	// Start (40, 24) left 1
	curr = data->distances[LEFT][0];
	x = 40 - ((20 - curr) * 2);
	y = 24;
	display[y][x] = 'X';

	// Start (40, 23) up 1 left 8
	curr = data->distances[LEFT][1];
	x = 40 - ((20 - curr) * 2);
	y = 23 - ((20 - curr) / 8);
	display[y][x] = 'X';

	// Start (40, 22) up 1 left 4
	curr = data->distances[LEFT][2];
	x = 40 - ((20 - curr) * 2);
	y = 22 - ((20 - curr) / 4);
	display[y][x] = 'X';

	// Start (40, 21) up 1 left 3
	curr = data->distances[LEFT][3];
	x = 40 - ((20 - curr) * 2);
	y = 21 - ((20 - curr) / 3);
	display[y][x] = 'X';

	// Start (40, 20) up 1 left 2
	curr = data->distances[LEFT][4];
	x = 40 - ((20 - curr) * 2);
	y = 20 - ((20 - curr) / 2);
	display[y][x] = 'X';

	// Start (41, 20) up 2 left 3
	curr = data->distances[LEFT][5];
	x = 41 - ((20 - curr) * 2);
	y = 20 - ((20 - curr) * 2 / 3);
	display[y][x] = 'X';

	// Start (42, 20) up 1 left 1
	curr = data->distances[LEFT][6];
	x = 42 - (20 - curr);
	y = 20 - (20 - curr);
	display[y][x] = 'X';

	// Start (43, 20) up 3 left 2
	curr = data->distances[LEFT][7];
	x = 43 - ((20 - curr) * 2 / 3);
	y = 20 - (20 - curr);
	display[y][x] = 'X';

	// Start (44, 20) up 4 left 1
	curr = data->distances[LEFT][8];
	x = 44 - ((20 - curr) / 4);
	y = 20 - (20 - curr);
	display[y][x] = 'X';

	printf("Exit addObstable\n");
}
