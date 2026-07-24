/*
 * ex2_container_of.c — the most important macro in the kernel.
 *
 * Given a pointer to a MEMBER embedded in a struct, recover a pointer
 * to the WHOLE struct. The kernel hands your callbacks pointers to
 * generic embedded objects (cdev, list_head, work_struct, kobject...)
 * and container_of gets you back to your own device structure.
 */
#include <stdio.h>
#include <stddef.h> /* offsetof */

/* The real kernel version adds type-checking; this is the arithmetic core: */
#define container_of(ptr, type, member) \
	((type *)((char *)(ptr) - offsetof(type, member)))

/* A generic object some "subsystem" knows about... */
struct cdev {
	int dev_number;
};

/* ...embedded inside OUR device struct, which the subsystem knows
 * nothing about. */
struct my_device {
	char        name[16];
	int         irq;
	struct cdev cdev;     /* embedded, NOT a pointer */
	unsigned    reg_base;
};

/*
 * A callback as the kernel would invoke it: the subsystem only has the
 * embedded cdev, but we need our full my_device.
 */
static void subsystem_callback(struct cdev *cdev)
{
	struct my_device *dev = container_of(cdev, struct my_device, cdev);

	printf("callback got cdev %p (dev_number=%d)\n",
	       (void *)cdev, cdev->dev_number);
	printf("  -> recovered my_device %p: name=%s irq=%d\n",
	       (void *)dev, dev->name, dev->irq);
}

int main(void)
{
	struct my_device dev = {
		.name     = "gpio-expander",
		.irq      = 42,
		.cdev     = { .dev_number = 7 },
		.reg_base = 0x4000,
	};

	printf("layout of struct my_device (%zu bytes):\n", sizeof(dev));
	printf("  offsetof(name)     = %2zu\n", offsetof(struct my_device, name));
	printf("  offsetof(irq)      = %2zu\n", offsetof(struct my_device, irq));
	printf("  offsetof(cdev)     = %2zu\n", offsetof(struct my_device, cdev));
	printf("  offsetof(reg_base) = %2zu\n", offsetof(struct my_device, reg_base));

	printf("\n&dev       = %p\n", (void *)&dev);
	printf("&dev.cdev  = %p  (= &dev + %zu)\n",
	       (void *)&dev.cdev, offsetof(struct my_device, cdev));

	printf("\ncontainer_of subtracts that offset back out:\n");
	subsystem_callback(&dev.cdev);

	return 0;
}
