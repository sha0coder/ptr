#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/ioctl.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/uaccess.h>
#include <linux/version.h>
#include "iocomm.h"

#define DEVICE_NAME "ptr"
#define CLASS_NAME "ptr_class"

static int majorNumber;
static struct class*  ptrClass  = NULL;
static struct cdev ptr_cdev;

static int dev_open(struct inode *, struct file *);
static int dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);
static long dev_ioctl(struct file *, unsigned int, unsigned long);

extern struct module *find_module_by_name(const char *name);

struct file_operations fops = {
    .open = dev_open,
    .read = dev_read,
    .write = dev_write,
    .release = dev_release,
    .unlocked_ioctl = dev_ioctl,
};

int init_device(void) {

    majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
    if (majorNumber < 0) {
        printk(KERN_ALERT "PTR failed registering major number\n");
        return majorNumber;
    }

    //#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,XX)
    //ptrClass = class_create(THIS_MODULE, CLASS_NAME);
    //#else
    ptrClass = class_create(CLASS_NAME);
    //#endif

    if (IS_ERR(ptrClass)) {
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "PTR failed registering the device class\n");
        return PTR_ERR(ptrClass);
    }

    if (device_create(ptrClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME) == NULL) {
        class_destroy(ptrClass);
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "PTR failed creating device\n");
        return -1;
    }

    cdev_init(&ptr_cdev, &fops);
    if (cdev_add(&ptr_cdev, MKDEV(majorNumber, 0), 1) == -1) {
        device_destroy(ptrClass, MKDEV(majorNumber, 0));
        class_destroy(ptrClass);
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "PTR failed adding cdev\n");
        return -1;
    }

    return 0;
}

void cleanup_device(void) {
    cdev_del(&ptr_cdev);
    device_destroy(ptrClass, MKDEV(majorNumber, 0));
    class_destroy(ptrClass);
    unregister_chrdev(majorNumber, DEVICE_NAME);
}

static int dev_open(struct inode *inodep, struct file *filep) {
    printk(KERN_INFO "PTR: Dispositivo abierto\n");
    return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset) {
    printk(KERN_INFO "PTR: Leyendo desde el dispositivo\n");
    return 0;
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset) {
    printk(KERN_INFO "PTR: Escribiendo en el dispositivo\n");
    return len;
}

static int dev_release(struct inode *inodep, struct file *filep) {
    printk(KERN_INFO "PTR: Dispositivo cerrado\n");
    return 0;
}

static long dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    switch (cmd) {
        case IOCTL_SET_MSG:
            printk(KERN_INFO "PTR: IOCTL set message\n");
            break;
        case IOCTL_GET_MSG:
            printk(KERN_INFO "PTR: IOCTL get message\n");
            break;
        default:
            return -ENOTTY;
    }
    return 0;
}

