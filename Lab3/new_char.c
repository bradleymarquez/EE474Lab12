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
	// (1) ALLOCATE DINAMICALLY TO ASSIGN OUR DEVICE
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

	usleep(10000);

	gpio_request(45, "Data");
	gpio_request(47, "Latch");
	gpio_request(67, "Clock");

	gpio_export(45, true);
	gpio_export(47, true);
	gpio_export(67, true);

	return 0;
}

// called up exit.
// unregisters the device and all associated gpios with it.
static void __exit driver_exit(void) {
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
    char val = *bufSource;

    shiftRegister(val);

	printk(KERN_INFO "new_char: writing to device...\n");
	return copy_from_user(virtual_device.data, bufSource, bufCount);
}

// shift register module
void shiftRegister(char num) {
    int i = 0;
    int j = 0;
    int binary[8];


}

MODULE_LICENSE("GPL"); // module license: required to use some functionalities.
module_init(driver_entry); // declares which function runs on init.
module_exit(driver_exit);  // declares which function runs on exit.
