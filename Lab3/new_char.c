/*
 * new_char.c: holds a buffer of 100 characters as device file.
 *             prints out contents of buffer on read.
 *             writes over buffer values on write.
 */
#include "new_char.h"
#include <linux/gpio.h>
#include <linux/delay.h>

#define DATA_ 45
#define LATCH_ 47
#define CLOCK_ 67
#define RS_ 68
#define RW_ 44
#define E_ 26
#define E2_ 46

#define CHAR_PER_LINE 16
#define NUM_LINES 2

/********************* FILE OPERATION FUNCTIONS ***************/

// runs on startup
// intializes module space and declares major number.
// assigns device structure data.
static int __init driver_entry(void) {
	// REGISTERIONG OUR DEVICE WITH THE SYSTEM
	// ALLOCATE DYNAMICALLY TO ASSIGN OUR DEVICE
	int ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
	if (ret < 0) {
		printk(KERN_ALERT "new_char: Failed to allocate a major number\n");
		return ret;
	}
	printk(KERN_INFO "new_char: major number is %d\n", MAJOR(dev_num));
	printk(KERN_INFO "Use mknod /dev/%s c %d 0 for device file\n", DEVICE_NAME, MAJOR(dev_num));

	// CREATE CDEV STRUCTURE, INITIALIZING CDEV
	mcdev = cdev_alloc();
	mcdev->ops = &fops;
	mcdev->owner = THIS_MODULE;

	// After creating cdev, add it to kernel
	ret = cdev_add(mcdev, dev_num, 1);
	if (ret < 0) {
		printk(KERN_ALERT "new_char: unable to add cdev to kernerl\n");
		return ret;
	}
	
	// Initialize SEMAPHORE
	sema_init(&virtual_device.sem, 1);
	msleep(10);
	
	return 0;
}

// called up exit.
// unregisters the device and all associated gpios with it.
static void __exit driver_exit(void) {
	cdev_del(mcdev);
	unregister_chrdev_region(dev_num, 1);

}

// Called on device file open
//	inode reference to file on disk, struct file represents an abstract
// checks to see if file is already open (semaphore is in use)
// prints error message if device is busy.
int device_open(struct inode *inode, struct file* filp) {
	if (down_interruptible(&virtual_device.sem) != 0) {
		printk(KERN_ALERT "new_char: could not lock device during open\n");
		return -1;
	}

	// Request access to all the needed GPIO pins
	gpio_request(DATA_, "Data");
	gpio_request(LATCH_, "Latch");
	gpio_request(CLOCK_, "Clock");
	gpio_request(RS_, "RS");
	gpio_request(RW_, "R/W");
	gpio_request(E_, "E");
	gpio_request(E2_, "E2");

	// Set all pins for output
	gpio_direction_output(DATA_, 0);
	gpio_direction_output(LATCH_, 0);
	gpio_direction_output(CLOCK_, 0);
	gpio_direction_output(RS_, 0);
	gpio_direction_output(RW_, 0);
	gpio_direction_output(E_, 0);
	gpio_direction_output(E2_, 0);
	
	initialize1();
	initialize2();

	return 0;
}

// Called upon close
// closes device, clear display, free the GPIO pins, and returns access to semaphore.
int device_close(struct inode* inode, struct  file *filp) {
	up(&virtual_device.sem);
	clearDisplay1();	
	displayOff1();
	clearDisplay2();	
	displayOff2();
	gpio_free(DATA_);
	gpio_free(LATCH_);
	gpio_free(CLOCK_);
	gpio_free(RS_);
	gpio_free(RW_);
	gpio_free(E_);
	gpio_free(E2_);
	return 0;
}

// Called when user wants to get info from device file
ssize_t device_read(struct file* filp, char* bufStoreData, size_t bufCount, loff_t* curOffset) {
	return copy_to_user(bufStoreData, virtual_device.data, bufCount);
}

// Called when user wants to send info to device
// Calling a shift register file
ssize_t device_write(struct file* filp, const char* bufSource, size_t bufCount, loff_t* curOffset) {
	clearDisplay();
	int firstLine = 0, secondLine = 0;

	// Determine how many lines of the diplay will be used
	if (bufCount > CHAR_PER_LINE * NUM_LINES) {
		firstLine = CHAR_PER_LINE;
		secondLine = CHAR_PER_LINE;
	} else if (bufCount > CHAR_PER_LINE) {
		firstLine = CHAR_PER_LINE;
		secondLine = bufCount - CHAR_PER_LINE;
	} else {
		firstLine = bufCount;
		secondLine = 0;
	}

	// Write to the first line of display
	int i = 0;
	for (i = 0; i < firstLine; i++) {
		writeChar(bufSource[i]);
	}

	// Write to the second line
	if (bufCount > CHAR_PER_LINE) setAddress((unsigned char) 0x40);
	for (i = 0; i < secondLine; i++) {
		writeChar(bufSource[i + CHAR_PER_LINE]);
	}

	return copy_from_user(virtual_device.data, bufSource, bufCount);
}

// Initializes the LCD with two lines
void initialize2() {
	gpio_set_value(RS_, 0);
	gpio_set_value(RW_, 0);

	msleep(15);
	
	command2((unsigned char) 0x30); // Function Set #1
	msleep(5);
	
	lcdSend2(); // Function Set #2
	msleep(1);

	lcdSend2(); // Function Set #3
	msleep(1);

	command2((unsigned char) 0x38); // Function Set #4
	msleep(1);

	command2((unsigned char) 0x08); // Display OFF
	udelay(50);

	command2((unsigned char) 0x01); // Clear Display
	msleep(16);
	
	command2((unsigned char) 0x06); // Entry Mode Set
	udelay(50);
	
	command2((unsigned char) 0x0c); // Display ON
	udelay(50);
}

// Initializes the LCD with one line
void initialize1() {
	gpio_set_value(RS_, 0);
	gpio_set_value(RW_, 0);

	msleep(15);
	
	command1((unsigned char) 0x30); // Function Set #1
	msleep(5);
	
	lcdSend1(); // Function Set #2
	msleep(1);

	lcdSend1(); // Function Set #3
	msleep(1);

	command1((unsigned char) 0x34); // Function Set #4
	msleep(1);

	command1((unsigned char) 0x08); // Display OFF
	udelay(50);

	command1((unsigned char) 0x01); // Clear Display
	msleep(16);

	command1((unsigned char) 0x05); // Entry Mode Set
	udelay(50);

	command1((unsigned char) 0x0c); // Display ON
	udelay(50);
}

// Loads data through the shift register and sends the command to the LCD
void command1(unsigned char data) {
	setBus(data); // Display on w/ cursor & blink on
	lcdSend1();
}

// Loads data through the shift register and sends the command to the LCD
void command2(unsigned char data) {
	setBus(data); // Display on w/ cursor & blink on
	lcdSend2();
}

// Loads and sends data into and from the shift register
void setBus(unsigned char num) {
	int i = 7;
	int j = 0;
	int binary[8];
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

}

// Clears the LCD
void clearDisplay1(){
	gpio_set_value(RS_, 0);
	gpio_set_value(RW_, 0);
	command1 ((unsigned char) 0x01); // Clear Display
	msleep(16);
}

// Clears the LCD
void clearDisplay2(){
	gpio_set_value(RS_, 0);
	gpio_set_value(RW_, 0);
	command2 ((unsigned char) 0x01); // Clear Display
	msleep(16);
}

// Turns the LCD off
void displayOff1() {
	gpio_set_value(RS_, 0);
	gpio_set_value(RW_, 0);
	command1 ((unsigned char) 0x08); // Display OFF
	udelay(50);
}

// Turns the LCD off
void displayOff2() {
	gpio_set_value(RS_, 0);
	gpio_set_value(RW_, 0);
	command2 ((unsigned char) 0x08); // Display OFF
	udelay(50);
}

// Sets the R/W pointer to the address specified
void setAddress1(unsigned char address) {
	gpio_set_value(RS_, 0);
	gpio_set_value(RW_, 0);
	address |= 0x80;
	setBus(address);
	lcdSend1();
	udelay(50);
}

// Sets the R/W pointer to the address specified
void setAddress2(unsigned char address) {
	gpio_set_value(RS_, 0);
	gpio_set_value(RW_, 0);
	address |= 0x80;
	setBus(address);
	lcdSend2();
	udelay(50);
}

// Sets DB7 to DB0 to the given 8 bits
void writeChar1(unsigned char character) {
	gpio_set_value(RS_, 1);
	gpio_set_value(RW_, 0);
	setBus(character);
	lcdSend2();
	udelay(50);
}

// Sets DB7 to DB0 to the given 8 bits
void writeChar2(unsigned char character) {
	gpio_set_value(RS_, 1);
	gpio_set_value(RW_, 0);
	setBus(character);
	lcdSend2();
	udelay(50);
}

// Flips the enable switch on the 1-line LCD to execute the loaded instruction
void lcdSend1() {
	gpio_set_value(E2_, 1);	// flip enable high
	udelay(50);
	gpio_set_value(E2_, 0); // sends on falling edge
}

// Flips the enable switch on the 2-line LCD to execute the loaded instruction
void lcdSend2() {
	gpio_set_value(E_, 1);	// flip enable high
	udelay(50);
	gpio_set_value(E_, 0); // sends on falling edge
}

MODULE_LICENSE("GPL"); // module license: required to use some functionalities.
module_init(driver_entry); // declares which function runs on init.
module_exit(driver_exit);  // declares which function runs on exit.

