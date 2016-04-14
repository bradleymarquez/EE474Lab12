/*
 * Method headers for music.c
 */

#include <stdio.h>

// This function takes two files pointes and two ints. note will be written
// to dirT and dutyc will be written to dirduty. This allows convenient
// way to change both the duty cycle and frequency of a PWM pin.
void soundChange(FILE *dirduty, FILE *dirT, int note, int dutyc);
