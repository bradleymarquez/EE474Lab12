#ifndef _LEDINIT_H_
#define _LEDINIT_H_

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>

// Total number of pins used with LCD
#define TOTALPINS 11
// Number of character spaces on LCD
#define SCREEN_SIZE 32

// Pin location in direction/value array
#define RS 0
#define RW 1
#define E 2
#define DB0 3
#define DB1 4
#define DB2 5
#define DB3 6
#define DB4 7
#define DB5 8
#define DB6 9
#define DB7 10

#define LCD_4 48 // RS Pin - GPIO_PIN_48
#define LCD_5 49 // RW Pin - GPIO_PIN_49
#define LCD_6 60 // E Pin - GPIO_PIN_60
#define LCD_7 66 // DB0 Pin - GPIO_PIN_66
#define LCD_8 69 // DB1 Pin - GPIO_PIN_69
#define LCD_9 45 // DB2 Pin - GPIO_PIN_45
#define LCD_10 47 // DB3 Pin - GPIO_PIN_47
#define LCD_11 67 // DB4 Pin - GPIO_PIN_67
#define LCD_12 68 // DB5 Pin - GPIO_PIN_68
#define LCD_13 44 // DB6 Pin - GPIO_PIN_44
#define LCD_14 26 // DB7 Pin - GPIO_PIN_26
#define TEST 112 // Used for testing only

extern int fd;

void clearDisplay();
void displayOff();
void closeLCD();
int lcdBoot();
int printScreen();

#endif  // _LEDINIT_H_
