/* 
 * lcd_driver.h: holds all include statements, preprocessor constants
 *             data structures, global variables, and function prototypes
 *             used throughout this file.
 *
 * general rule:
 * global variables should be declared static,
 * including global variables within the file.
 */
#ifndef LCD_DRIVER_H_
#define LCD_DRIVER_H_
 
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/semaphore.h>
#include <asm/uaccess.h>
#include <linux/unistd.h>

#define DEVICE_NAME "lcd_driver"

/* data structures */
// contains data about the device.
// data : buffer for character data stored.
struct fake_device {
	char data[100];
	struct semaphore sem;
} virtual_device;

/* global variables */
// stores info about this char device.
static struct cdev* mcdev;
// holds major and minor number granted by the kernel
static dev_t dev_num;

/* function prototypes */
// file operations
static int __init driver_entry(void);
static void __exit driver_exit(void);
static int  device_open(struct inode*, struct file*);
static int device_close(struct inode*, struct file *);
static ssize_t device_read(struct file*, char*, size_t, loff_t*);
static ssize_t device_write(struct file*, const char*, size_t, loff_t*);

void setBus(unsigned char);
void command(unsigned char);
void shiftRegister(unsigned char);
void lcdSend(void);
void initialize(void);
void displayOff(void);
void clearDisplay(void);
void writeChar(unsigned char);
void setAddress(unsigned char);

/* operations usable by this file. */
static struct file_operations fops = {
   .owner = THIS_MODULE,
   .read = device_read,
   .write = device_write,
   .open = device_open,
   .release = device_close
};
#endif  // LCD_DRIVER_H_
