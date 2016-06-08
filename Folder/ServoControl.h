/*	ServoControl.h
 * Brad Marquez, Joseph Rothlin, Aigerim Shintemirova
 * 08 / June / 2016
 */

#ifndef _SERVOCONTROL_H_
#define _SERVOCONTROL_H_

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>

static FILE *sys, *dirduty, *dirT;

// Initializes the PWM files used to control the servo
bool servoInit();

// Sets the angle of the servo from the "minimum" position.
// Can range from 0 to 120, takes values in degrees.
// 0-15 degrees has hardware problem (it's more like a difference of 5 degrees)
// but everything is normal past that value
void angleFromZero(int angle);

// Closes files that were opened from servoInit()
void closeServo();

#endif  // _SERVOCONTROL_H_
