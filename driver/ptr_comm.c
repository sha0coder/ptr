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
static unsigned long long *glob_seek = 0;

static int dev_open(struct inode *, struct file *);
static int dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);
static long dev_ioctl(struct file *, unsigned int, unsigned long);
static loff_t dev_seek(struct file *filep, loff_t offset, int whence);

extern struct module *find_module_by_name(const char *name);

struct file_operations fops = {
    .open = dev_open,
    .read = dev_read,
    .write = dev_write,
    .release = dev_release,
    .llseek = dev_seek,
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

// file operations

static int dev_open(struct inode *inodep, struct file *filep) {
    return 0;
}

static loff_t dev_seek(struct file *filep, loff_t offset, int whence) {
    loff_t new_pos = 0;

    switch (whence) {
    case SEEK_SET:
        break;
    case SEEK_CUR:
        glob_seek = (unsigned long long *)offset;
        break;
    case SEEK_END:
        break;
    default:
        return -EINVAL;
    }

    if (new_pos < 0) return -EINVAL;
    filep->f_pos = new_pos;
    return new_pos;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset) {
    return 0;
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset) {
    return len;
}

static int dev_release(struct inode *inodep, struct file *filep) {
    return 0;
}

static long dev_ioctl(struct file *filep, unsigned int cmd, unsigned long arg) {
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

