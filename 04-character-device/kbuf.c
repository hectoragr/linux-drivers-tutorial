// SPDX-License-Identifier: GPL-2.0
/*
 * kbuf.c — a character device backed by a kernel buffer.
 *
 * Creates /dev/kbuf0: a 4 KiB buffer you can read/write like a file.
 * Demonstrates the full char-device plumbing: dev_t, cdev, file_operations,
 * class/device (udev), and copy_to_user/copy_from_user.
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>      /* alloc_chrdev_region, file_operations */
#include <linux/cdev.h>    /* struct cdev */
#include <linux/device.h>  /* class_create, device_create */
#include <linux/uaccess.h> /* copy_to_user, copy_from_user */
#include <linux/mutex.h>
#include <linux/slab.h>

#define KBUF_SIZE 4096
#define KBUF_NAME "kbuf"

/* Everything about one device instance lives here. The cdev is EMBEDDED
 * (not a pointer) so open() can container_of() its way back to us. */
struct kbuf_dev {
	struct cdev  cdev;
	struct mutex lock;    /* guards buf and size; see module 07 */
	char         buf[KBUF_SIZE];
	size_t       size;    /* bytes of valid data in buf */
};

static dev_t         kbuf_devt;   /* our (major, minor) base */
static struct class *kbuf_class;
static struct kbuf_dev kbuf_device;

/* ---- file operations ---------------------------------------------------- */

static int kbuf_open(struct inode *inode, struct file *file)
{
	/* The VFS gives us the generic cdev; recover OUR struct around it.
	 * (With N minors sharing these fops, this line is what finds the
	 * right instance — module 01 ex2, earning its keep.) */
	struct kbuf_dev *dev = container_of(inode->i_cdev,
					    struct kbuf_dev, cdev);

	file->private_data = dev;
	pr_debug("kbuf: open by '%s' (pid %d)\n", current->comm, current->pid);
	return 0;
}

static int kbuf_release(struct inode *inode, struct file *file)
{
	return 0;
}

static ssize_t kbuf_read(struct file *file, char __user *ubuf,
			 size_t count, loff_t *ppos)
{
	struct kbuf_dev *dev = file->private_data;
	ssize_t ret;

	if (mutex_lock_interruptible(&dev->lock))
		return -ERESTARTSYS;

	if (*ppos >= dev->size) {
		ret = 0;              /* EOF: this is how `cat` stops */
		goto out;
	}

	/* Clamp to what's actually available past the offset. */
	if (count > dev->size - *ppos)
		count = dev->size - *ppos;

	if (copy_to_user(ubuf, dev->buf + *ppos, count)) {
		ret = -EFAULT;        /* bad user pointer: their problem */
		goto out;
	}

	*ppos += count;               /* the other half of the protocol */
	ret = count;
out:
	mutex_unlock(&dev->lock);
	return ret;
}

static ssize_t kbuf_write(struct file *file, const char __user *ubuf,
			  size_t count, loff_t *ppos)
{
	struct kbuf_dev *dev = file->private_data;
	ssize_t ret;

	if (mutex_lock_interruptible(&dev->lock))
		return -ERESTARTSYS;

	if (*ppos >= KBUF_SIZE) {
		ret = -ENOSPC;        /* policy choice; exercise 4 changes it */
		goto out;
	}

	if (count > KBUF_SIZE - *ppos)
		count = KBUF_SIZE - *ppos;

	if (copy_from_user(dev->buf + *ppos, ubuf, count)) {
		ret = -EFAULT;
		goto out;
	}

	*ppos += count;
	if (*ppos > dev->size)
		dev->size = *ppos;    /* grow the valid region */
	ret = count;
out:
	mutex_unlock(&dev->lock);
	return ret;
}

static loff_t kbuf_llseek(struct file *file, loff_t off, int whence)
{
	/* Lets `dd seek=` and friends work. Bounds-checked helper from the
	 * kernel: seeks are confined to [0, KBUF_SIZE]. */
	return fixed_size_llseek(file, off, whence, KBUF_SIZE);
}

static const struct file_operations kbuf_fops = {
	.owner   = THIS_MODULE,   /* pins the module while a fd is open */
	.open    = kbuf_open,
	.release = kbuf_release,
	.read    = kbuf_read,
	.write   = kbuf_write,
	.llseek  = kbuf_llseek,
};

/* ---- module init/exit ---------------------------------------------------
 *
 * Init acquires four resources in order; each failure unwinds ONLY what
 * was already acquired, via the goto ladder. Exit is the exact mirror.
 */
static int __init kbuf_init(void)
{
	struct device *device;
	int ret;

	/* 1. Ask for a dynamic major, 1 minor, starting at minor 0. */
	ret = alloc_chrdev_region(&kbuf_devt, 0, 1, KBUF_NAME);
	if (ret)
		return ret;

	/* 2. Wire our fops to that dev_t. After cdev_add, open() can be
	 *    called AT ANY MOMENT — everything must be ready before it. */
	mutex_init(&kbuf_device.lock);
	cdev_init(&kbuf_device.cdev, &kbuf_fops);
	kbuf_device.cdev.owner = THIS_MODULE;
	ret = cdev_add(&kbuf_device.cdev, kbuf_devt, 1);
	if (ret)
		goto err_unregister;

	/* 3+4. Publish to sysfs so udev creates /dev/kbuf0. */
	kbuf_class = class_create(KBUF_NAME);
	if (IS_ERR(kbuf_class)) {
		ret = PTR_ERR(kbuf_class);
		goto err_cdev;
	}

	device = device_create(kbuf_class, NULL, kbuf_devt, NULL,
			       KBUF_NAME "0");
	if (IS_ERR(device)) {
		ret = PTR_ERR(device);
		goto err_class;
	}

	pr_info("kbuf: ready at /dev/%s0 (major %d, minor %d)\n",
		KBUF_NAME, MAJOR(kbuf_devt), MINOR(kbuf_devt));
	return 0;

err_class:
	class_destroy(kbuf_class);
err_cdev:
	cdev_del(&kbuf_device.cdev);
err_unregister:
	unregister_chrdev_region(kbuf_devt, 1);
	return ret;
}

static void __exit kbuf_exit(void)
{
	device_destroy(kbuf_class, kbuf_devt);
	class_destroy(kbuf_class);
	cdev_del(&kbuf_device.cdev);
	unregister_chrdev_region(kbuf_devt, 1);
	pr_info("kbuf: unloaded\n");
}

module_init(kbuf_init);
module_exit(kbuf_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Hector Gomez");
MODULE_DESCRIPTION("Tutorial module 04: character device with a kernel buffer");
