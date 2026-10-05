/*
 * parking_sensor.c – Linux Kernel Character Device Driver
 * Smart Parking Management System
 *
 * Simulates ultrasonic/IR parking slot sensors.
 * Registers as a character device at /dev/parking_sensor.
 *
 * Supports:
 *   - read()  : returns current slot status as binary SensorData structs
 *   - write() : accepts slot update commands
 *   - ioctl() : PARKING_GET_STATUS, SET_OCCUPIED, SET_FREE, GET_COUNT
 *
 * Build: cd driver && make
 * Load:  sudo insmod parking_sensor.ko
 * Check: dmesg | tail -20
 * Node:  sudo mknod /dev/parking_sensor c <major> 0
 *        OR use udev (auto-created via class_device_create)
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/ktime.h>

#include "parking_sensor.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Student - Smart Parking System");
MODULE_DESCRIPTION("Parking slot sensor character device driver");
MODULE_VERSION("1.0");

/* ──────────────────────────────── globals ──────────────────────────────── */

#define DEVICE_NAME "parking_sensor"
#define CLASS_NAME  "parking"

static int           major_number;
static struct class  *parking_class  = NULL;
static struct device *parking_device = NULL;
static struct cdev    parking_cdev;

/* Slot state array – shared between kernel and userspace */
static int slot_status[MAX_SLOTS];   /* 0=free, 1=occupied */
static int total_slots = MAX_SLOTS;

static DEFINE_MUTEX(sensor_mutex);

/* Read position for sequential reads */
static int read_pos = 0;

/* ──────────────────────────────── file ops ─────────────────────────────── */

static int parking_open(struct inode *inode, struct file *file)
{
    mutex_lock(&sensor_mutex);
    read_pos = 0;          /* reset read pointer on open */
    mutex_unlock(&sensor_mutex);
    printk(KERN_INFO "parking_sensor: device opened\n");
    return 0;
}

static int parking_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "parking_sensor: device closed\n");
    return 0;
}

/*
 * read() – returns SensorData structs for all slots.
 * Userspace reads one SensorData at a time (sequential).
 */
static ssize_t parking_read(struct file *file, char __user *buf,
                             size_t len, loff_t *offset)
{
    struct SensorData data;
    int ret;

    if (*offset >= total_slots * (loff_t)sizeof(struct SensorData))
        return 0;   /* EOF */

    if (len < sizeof(struct SensorData))
        return -EINVAL;

    mutex_lock(&sensor_mutex);
    {
        int slot = (int)(*offset / sizeof(struct SensorData));
        data.slotId    = slot;
        data.status    = slot_status[slot];
        data.timestamp = ktime_get_real_seconds();
    }
    mutex_unlock(&sensor_mutex);

    ret = copy_to_user(buf, &data, sizeof(struct SensorData));
    if (ret) return -EFAULT;

    *offset += sizeof(struct SensorData);
    return sizeof(struct SensorData);
}

/*
 * write() – accepts SensorData to update slot status from userspace/simulator.
 */
static ssize_t parking_write(struct file *file, const char __user *buf,
                              size_t len, loff_t *offset)
{
    struct SensorData data;

    if (len < sizeof(struct SensorData))
        return -EINVAL;

    if (copy_from_user(&data, buf, sizeof(struct SensorData)))
        return -EFAULT;

    if (data.slotId < 0 || data.slotId >= MAX_SLOTS)
        return -EINVAL;

    mutex_lock(&sensor_mutex);
    slot_status[data.slotId] = data.status ? 1 : 0;
    mutex_unlock(&sensor_mutex);

    printk(KERN_INFO "parking_sensor: slot %d set to %s\n",
           data.slotId, data.status ? "OCCUPIED" : "FREE");

    return sizeof(struct SensorData);
}

/*
 * ioctl() – control interface.
 */
static long parking_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    int slot, value, free_count, i;

    switch (cmd) {
    case PARKING_GET_STATUS:
        if (copy_from_user(&slot, (int __user *)arg, sizeof(int)))
            return -EFAULT;
        if (slot < 0 || slot >= MAX_SLOTS) return -EINVAL;
        mutex_lock(&sensor_mutex);
        value = slot_status[slot];
        mutex_unlock(&sensor_mutex);
        if (copy_to_user((int __user *)arg, &value, sizeof(int)))
            return -EFAULT;
        break;

    case PARKING_SET_OCCUPIED:
        if (copy_from_user(&slot, (int __user *)arg, sizeof(int)))
            return -EFAULT;
        if (slot < 0 || slot >= MAX_SLOTS) return -EINVAL;
        mutex_lock(&sensor_mutex);
        slot_status[slot] = 1;
        mutex_unlock(&sensor_mutex);
        printk(KERN_INFO "parking_sensor: ioctl – slot %d OCCUPIED\n", slot);
        break;

    case PARKING_SET_FREE:
        if (copy_from_user(&slot, (int __user *)arg, sizeof(int)))
            return -EFAULT;
        if (slot < 0 || slot >= MAX_SLOTS) return -EINVAL;
        mutex_lock(&sensor_mutex);
        slot_status[slot] = 0;
        mutex_unlock(&sensor_mutex);
        printk(KERN_INFO "parking_sensor: ioctl – slot %d FREE\n", slot);
        break;

    case PARKING_GET_COUNT:
        free_count = 0;
        mutex_lock(&sensor_mutex);
        for (i = 0; i < total_slots; i++)
            if (slot_status[i] == 0) free_count++;
        mutex_unlock(&sensor_mutex);
        if (copy_to_user((int __user *)arg, &free_count, sizeof(int)))
            return -EFAULT;
        break;

    default:
        return -ENOTTY;
    }
    return 0;
}

/* ──────────────────────────────── file_operations ──────────────────────── */

static struct file_operations parking_fops = {
    .owner          = THIS_MODULE,
    .open           = parking_open,
    .release        = parking_release,
    .read           = parking_read,
    .write          = parking_write,
    .unlocked_ioctl = parking_ioctl,
};

/* ──────────────────────────────── init / exit ───────────────────────────── */

static int __init parking_sensor_init(void)
{
    int i, ret;
    dev_t dev;

    printk(KERN_INFO "parking_sensor: initializing module\n");

    /* Initialize all slots as free */
    for (i = 0; i < MAX_SLOTS; i++)
        slot_status[i] = 0;

    /* Allocate device number dynamically */
    ret = alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);
    if (ret < 0) {
        printk(KERN_ERR "parking_sensor: failed to alloc device number\n");
        return ret;
    }
    major_number = MAJOR(dev);
    printk(KERN_INFO "parking_sensor: registered with major number %d\n", major_number);

    /* Register character device */
    cdev_init(&parking_cdev, &parking_fops);
    parking_cdev.owner = THIS_MODULE;
    ret = cdev_add(&parking_cdev, dev, 1);
    if (ret < 0) {
        unregister_chrdev_region(dev, 1);
        printk(KERN_ERR "parking_sensor: cdev_add failed\n");
        return ret;
    }

    /* Create device class */
    parking_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(parking_class)) {
        cdev_del(&parking_cdev);
        unregister_chrdev_region(dev, 1);
        return PTR_ERR(parking_class);
    }

    /* Create device node /dev/parking_sensor automatically */
    parking_device = device_create(parking_class, NULL, dev, NULL, DEVICE_NAME);
    if (IS_ERR(parking_device)) {
        class_destroy(parking_class);
        cdev_del(&parking_cdev);
        unregister_chrdev_region(dev, 1);
        return PTR_ERR(parking_device);
    }

    printk(KERN_INFO "parking_sensor: device /dev/%s created\n", DEVICE_NAME);
    printk(KERN_INFO "parking_sensor: %d slots initialized as FREE\n", MAX_SLOTS);
    return 0;
}

static void __exit parking_sensor_exit(void)
{
    dev_t dev = MKDEV(major_number, 0);
    device_destroy(parking_class, dev);
    class_destroy(parking_class);
    cdev_del(&parking_cdev);
    unregister_chrdev_region(dev, 1);
    printk(KERN_INFO "parking_sensor: module unloaded\n");
}

module_init(parking_sensor_init);
module_exit(parking_sensor_exit);
