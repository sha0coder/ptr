#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/ioctl.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/uaccess.h>
#include "iocomm.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ptr kernel debugger");
MODULE_DESCRIPTION("Un ejemplo de driver minimalista de Linux");


extern int init_device(void);
extern void cleanup_device(void);


int init_module(void) {
    init_device();

    printk(KERN_INFO "ptr debugging driver loaded.\n");
    return 0;
}

void cleanup_module(void) {
    cleanup_device();

    printk(KERN_INFO "ptr debugging driver unloaded.\n");
}
