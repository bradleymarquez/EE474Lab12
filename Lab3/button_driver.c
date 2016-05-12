/*
 * button_driver.c: holds a buffer of 100 characters as device file.
 *             prints out contents of buffer on read.
 *             writes over buffer values on write.
 */
#include "button_driver.h"
#include <linux/gpio.h>
#include <linux/delay.h>

#define UP
#define DOWN
#define LEFT
#define RIGHT
#define PRESS

/********************* FILE OPERATION FUNCTIONS ***************/

// runs on startup
// intializes module space and declares major number.
// assigns device structure data.
static int __init driver_entry(void) {
	// REGISTERING OUR DEVICE WITH THE SYSTEM
	// ALLOCATE DYNAMICALLY TO ASSIGN OUR DEVICE
	int ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
	if (ret < 0) {
		printk(KERN_ALERT "button_driver: Failed to allocate a major number\n");
		return ret;
	}
	printk(KERN_INFO "button_driver: major number is %d\n", MAJOR(dev_num));
	printk(KERN_INFO "Use mknod /dev/%s c %d 0 for device file\n", DEVICE_NAME, MAJOR(dev_num));

	// CREATE CDEV STRUCTURE, INITIALIZING CDEV
	mcdev = cdev_alloc();
	mcdev->ops = &fops;
	mcdev->owner = THIS_MODULE;

	// After creating cdev, add it to kernel
	ret = cdev_add(mcdev, dev_num, 1);
	if (ret < 0) {
		printk(KERN_ALERT "button_driver: unable to add cdev to kernerl\n");
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
		printk(KERN_ALERT "button_driver: could not lock device during open\n");
		return -1;
	}

	// Request access to all the needed GPIO pins
	gpio_request(UP, "Up");
	gpio_request(DOWN, "Down");
	gpio_request(LEFT, "Left");
	gpio_request(RIGHT, "Right");
	gpio_request(PRESS, "Press");

	// Set all pins for output
	gpio_direction_input(UP);
	gpio_direction_input(DOWN);
	gpio_direction_input(LEFT);
	gpio_direction_input(RIGHT);
	gpio_direction_input(PRESS);

	return 0;
}

// Called upon close
// closes device, clear display, free the GPIO pins, and returns access to semaphore.
int device_close(struct inode* inode, struct  file *filp) {
	up(&virtual_device.sem);
	gpio_free(UP);
	gpio_free(DOWN);
	gpio_free(LEFT);
	gpio_free(RIGHT);
	gpio_free(PRESS);
	return 0;
}

// Called when user wants to get info from device file
ssize_t device_read(struct file* filp, char* bufStoreData, size_t bufCount, loff_t* curOffset) {
	virtual_device.status[0] = (char) (gpio_get_value(UP) + '0');
	virtual_device.status[1] = (char) (gpio_get_value(DOWN) + '0');
	virtual_device.status[2] = (char) (gpio_get_value(LEFT) + '0');
	virtual_device.status[3] = (char) (gpio_get_value(RIGHT) + '0');
	virtual_device.status[4] = (char) (gpio_get_value(PRESS) + '0');

	return copy_to_user(bufStoreData, virtual_device.status, NUM_BUTTONS);
}

// Called when user wants to send info to device
// Calling a shift register file
ssize_t device_write(struct file* filp, const char* bufSource, size_t bufCount, loff_t* curOffset) {
	return copy_from_user(virtual_device.data, bufSource, bufCount);
}

