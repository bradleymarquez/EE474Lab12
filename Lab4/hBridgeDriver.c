/*	ledinit.c
 * Brad Marquez, Joseph Rothlin, Aigerim Shintemirova
 * 22 / April / 2016
 *
 *	
 *  
 */

static FILE* sys, sys2, PWMA_T, PWMA_DUTY, PWMB_T, PWMA_DUTY, RS_VAL, RW_VAL, SER_DATA_VAL;
// CHANGE THESE
#define RS 48 // RS Pin - GPIO_PIN_48
#define RW 49 // RW Pin - GPIO_PIN_49
#define SER_DATA 67 // PIN

void pointSetup(void);
int main(){
	pointSetup();
	return 0;
}

void pointSetup(){
	sys = fopen("/sys/devices/bone_capemgr.9/slots", "w");
	fseek(sysA, 0, SEEK_END);
	fprintf(sysA, "am33xx_pwm");
	fflush(sysA);
	fprintf(sysA, "bone_pwm_P9_14"); // need to change???
	fflush(sys);
	
	PWMA_DUTY = fopen("/sys/devices/ocp.3/pwm_test_P9_14.15/duty", "w"); // change directory
	PWMA_T = fopen("/sys/devices/ocp.3/pwm_test_P9_14.15/period", "w"); // change directory
	
	PWMB_DUTY = fopen("/sys/devices/ocp.3/pwm_test_P9_14.15/duty", "w"); // change directory
	PWMB_T = fopen("/sys/devices/ocp.3/pwm_test_P9_14.15/period", "w"); // change directory
	
	changePWMA(10, 20); // what values of PWM?
	changePWMB(10, 20); // what values of PWM?
	
	sys2 = fopen("/sys/class/gpio/export", "w");
	fseek(sys2, 0, SEEK_SET);
	FILE * RS_dir, RW_dir, SER_dir;
	
	RS_dir = fopen("/sys/class/gpio/gpio48/direction", "w"); // change directory
	fseek(RS_dir, 0, SEEK_SET);
	fprintf(RS_dir, "%s", "out");
	fflush(RS_dir);
	
	RW_dir = fopen("/sys/class/gpio/gpio48/direction", "w"); // change directory
	fseek(RW_dir, 0, SEEK_SET);
	fprintf(RW_dir, "%s", "out");
	fflush(RW_dir);
	
	SER_dir = fopen("/sys/class/gpio/gpio48/direction", "w"); // change directory
	fseek(SER_dir, 0, SEEK_SET);
	fprintf(SER_dir, "%s", "out");
	fflush(SER_dir);
	
	RS_VAL = fopen("/sys/class/gpio/gpio48/value", "w"); // change directory
	RW_VAL = fopen("/sys/class/gpio/gpio48/value", "w"); // change directory
	SER_DATA_VAL = fopen("/sys/class/gpio/gpio48/value", "w"); // change directory
}

void changePWMA(int duty, int period) {
	fprintf(PWMA_DUTY, "%d", duty);
	fflush(PWMA_DUTY);
	fprintf(PWMA_T, "%d", period);
	fflush(PWMA_T);
}

void changePWMB(int duty, int period) {
	fprintf(PWMB_DUTY, "%d", duty);
	fflush(PWMB_DUTY);
	fprintf(PWMB_T, "%d", period);
	fflush(PWMB_T);
}

void command(unsigned char num) {
	fprintf(value[DB0], "%d", (byte % 2));
	fflush(value[DB0]);
	byte  = byte >> 1;
	fprintf(value[DB1], "%d", (byte % 2));
	fflush(value[DB1]);
	byte  = byte >> 1;
	fprintf(value[DB2], "%d", (byte % 2));
	fflush(value[DB2]);
	byte  = byte >> 1;
	fprintf(value[DB3], "%d", (byte % 2));
	fflush(value[DB3]);
	byte  = byte >> 1;
	fprintf(value[DB4], "%d", (byte % 2));
	fflush(value[DB4]);
	byte  = byte >> 1;
	fprintf(value[DB5], "%d", (byte % 2));
	fflush(value[DB5]);
	byte  = byte >> 1;
	fprintf(value[DB6], "%d", (byte % 2));
	fflush(value[DB6]);
	byte  = byte >> 1;
	fprintf(value[DB7], "%d", (byte % 2));
	fflush(value[DB7]);
}
/*
// Loads and sends data into and from the shift register
void setBus(unsigned char num) {
	int i = 7;
	int j = 0;
	int input[8];
	int temporary = num;

	// Building the binary version of num
	while (j < 8) {
		binary[j] = temporary % 2;
		temporary = temporary >> 1;
		j++;
	}

	// Inserting binary value into shift register
	while (i >= 0) {
		gpio_set_value(DATA_, binary[i]);  // Set the data line to the next value

		// Toggle the clock
		gpio_set_value(CLOCK_, 1);		
		udelay(10);
		gpio_set_value(CLOCK_, 0);
		i--;
	}
	
	// Toggle the latch
	gpio_set_value(LATCH_, 1);
	udelay(50);
	gpio_set_value(LATCH_, 0);

}*/

/*// Sets DB7 to DB0 to the given 8 bits
void setBus(unsigned char byte) {
	fprintf(value[DB0], "%d", (byte % 2));
	fflush(value[DB0]);
	byte  = byte >> 1;
	fprintf(value[DB1], "%d", (byte % 2));
	fflush(value[DB1]);
	byte  = byte >> 1;
	fprintf(value[DB2], "%d", (byte % 2));
	fflush(value[DB2]);
	byte  = byte >> 1;
	fprintf(value[DB3], "%d", (byte % 2));
	fflush(value[DB3]);
	byte  = byte >> 1;
	fprintf(value[DB4], "%d", (byte % 2));
	fflush(value[DB4]);
	byte  = byte >> 1;
	fprintf(value[DB5], "%d", (byte % 2));
	fflush(value[DB5]);
	byte  = byte >> 1;
	fprintf(value[DB6], "%d", (byte % 2));
	fflush(value[DB6]);
	byte  = byte >> 1;
	fprintf(value[DB7], "%d", (byte % 2));
	fflush(value[DB7]);
}*/