/*
 * new_char.c: holds a buffer of 100 characters as device file.
 *             prints out contents of buffer on read.
 *             writes over buffer values on write.
 */
#include "new_char.h"
#include <linux/gpio.h>
#include <linux/delay.h>

/********************* FILE OPERATION FUNCTIONS ***************/

// runs on startup
// intializes module space and declares major number.
// assigns device structure data.
static int __init driver_entry(void) {
	// REGISTERIONG OUR DEVICE WITH THE SYSTEM
	// (1) ALLOCATE DYNAMICALLY TO ASSIGN OUR DEVICE
	int ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
	if (ret < 0) {
		printk(KERN_ALERT "new_char: Failed to allocate a major number\n");
		return ret;
	}
	printk(KERN_INFO "new_char: major number is %d\n", MAJOR(dev_num));
	printk(KERN_INFO "Use mknod /dev/%s c %d 0 for device file\n", DEVICE_NAME, MAJOR(dev_num));

	// (2) CREATE CDEV STRUCTURE, INITIALIZING CDEV
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
	
	gpio_request(45, "Data");
	gpio_request(47, "Latch");
	gpio_request(67, "Clock");
	gpio_request(68, "RS");
	gpio_request(44, "R/W");
	gpio_request(26, "E");

	gpio_direction_output(45, 0);
	gpio_direction_output(47, 0);
	gpio_direction_output(67, 0);
	gpio_direction_output(68, 0);
	gpio_direction_output(44, 0);
	gpio_direction_output(26, 0);
	
	initialize();
	return 0;
}

// called up exit.
// unregisters the device and all associated gpios with it.
static void __exit driver_exit(void) {
	displayOff();
	clearDisplay();
	gpio_free(45);
	gpio_free(47);
	gpio_free(67);
	gpio_free(68);
	gpio_free(44);
	gpio_free(26);
	cdev_del(mcdev);
	unregister_chrdev_region(dev_num, 1);
	printk(KERN_ALERT "new_char: successfully unloaded\n");

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
	printk(KERN_INFO "new_char: device opened\n");
	return 0;
}

// Called upon close
// closes device and returns access to semaphore.
int device_close(struct inode* inode, struct  file *filp) {
	up(&virtual_device.sem);
	printk(KERN_INFO "new_char, closing device\n");
	return 0;
}

// Called when user wants to get info from device file
ssize_t device_read(struct file* filp, char* bufStoreData, size_t bufCount, loff_t* curOffset) {
	printk(KERN_INFO "new_char: Reading from device...\n");
	return copy_to_user(bufStoreData, virtual_device.data, bufCount);
}

// Called when user wants to send info to device
// Calling a shift register file
ssize_t device_write(struct file* filp, const char* bufSource, size_t bufCount, loff_t* curOffset) {
    unsigned char val = *bufSource;
	
    writeChar(val);

	printk(KERN_INFO "new_char: writing to device...\n");
	return copy_from_user(virtual_device.data, bufSource, bufCount);
}

// Initializes the LCD
void initialize() {
	gpio_direction_output(68, 0);
	gpio_direction_output(44, 0);

	msleep(15);
	
	command((unsigned char) 0x30); // Function Set #1
	msleep(5);
	
	lcdSend(); // Function Set #2
	msleep(1);

	lcdSend(); // Function Set #3
	msleep(1);

	command((unsigned char) 0x38); // Function Set #4
	msleep(1);

	command((unsigned char) 0x08); // Display OFF
	msleep(1);

	command((unsigned char) 0x01); // Clear Display
	msleep(16);

	command((unsigned char) 0x0c); // Entry Mode Set
	msleep(1);
}

// Loads data through the shift register and sends the command to the LCD
void command(unsigned char data) {
	setBus(data); // Display on w/ cursor & blink on
	lcdSend();
}

// Flips the enable switch on the LCD to execute the loaded instruction
void lcdSend() {
	gpio_direction_output(26, 1);	// flip enable high
	msleep(1);
	gpio_direction_output(26, 0); // sends on falling edge
}

// Clears the LCD
void clearDisplay(){
	gpio_direction_output(68, 0);
	gpio_direction_output(44, 0);
	command ((unsigned char) 0x01); // Clear Display
	msleep(1);
}

// Turns the LCD off
void displayOff() {
	gpio_direction_output(68, 0);
	gpio_direction_output(44, 0);
	command((unsigned char) 0x08); // Display OFF
	msleep(1);
}

// Loads and sends data into and from the shift register
void setBus(char num) {
	int i = 7;
	int j = 0;
	int binary[8];
    
	int temporary = num;

	while (j < 8) { // building binary number
		binary[j] = temporary % 2;
		temporary = temporary >> 1;
		j++;
	}

	/*printk("\n\n");
	for (j = 0; j < 8; j++) {
		printk("%d", binary[j]);
	}
	printk("\n\n");*/

	while (i >= 0) {
		gpio_set_value(45, binary[i]);	//shifting data bit at each clock transition

		gpio_set_value(67, 0);	// clock back on after the data bit is shifted
		msleep(1);

		gpio_set_value(67, 1);	// configure the default value of the output pin - clock off		
		msleep(1);

		gpio_set_value(67, 0);	// clock back on after the data bit is shifted
		msleep(1);

		i--; //the count
	}
	
	gpio_set_value(47, 0);
	msleep(1);
	gpio_set_value(47, 1);
	msleep(1);
	gpio_set_value(47, 0);
	msleep(1);
}

// Sets the R/W pointer to the address specified
void setAddress(unsigned char address) {
	gpio_direction_output(68, 0);
	gpio_direction_output(44, 0);
	address |= 0x80;
	setBus(address);
	lcdSend;
	msleep(1);
}

// Sets DB7 to DB0 to the given 8 bits
void writeChar(unsigned char character) {
	gpio_direction_output(68, 1);
	gpio_direction_output(44, 0);
	setBus(character);
	lcdSend;
	msleep(1);
}

MODULE_LICENSE("GPL"); // module license: required to use some functionalities.
module_init(driver_entry); // declares which function runs on init.
module_exit(driver_exit);  // declares which function runs on exit.

