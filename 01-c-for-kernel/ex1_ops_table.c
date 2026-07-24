/*
 * ex1_ops_table.c — polymorphism in C, the kernel way.
 *
 * A "subsystem" (generic code at the bottom) drives any device that
 * implements struct dev_ops. This is exactly how struct file_operations,
 * net_device_ops, snd_pcm_ops, etc. work in the kernel.
 */
#include <stdio.h>
#include <string.h>

/* ---- The interface: what every device must implement ------------------- */

struct dev_ops {
	int  (*open)(void *priv);
	long (*read)(void *priv, char *buf, unsigned long len);
	long (*write)(void *priv, const char *buf, unsigned long len);
	void (*release)(void *priv);
};

/* A "registered device": ops + private data, like the kernel pairs
 * file->f_op with file->private_data. */
struct device {
	const char           *name;
	const struct dev_ops *ops;
	void                 *priv;
};

/* ---- Implementation 1: a fake UART ------------------------------------- */

struct uart_state {
	unsigned int baud;
	unsigned int tx_count;
};

static int uart_open(void *priv)
{
	struct uart_state *u = priv;

	printf("  uart: opened at %u baud\n", u->baud);
	return 0;
}

static long uart_read(void *priv, char *buf, unsigned long len)
{
	(void)priv;
	/* Pretend the RX FIFO always holds "hello". */
	const char *rx = "hello";
	unsigned long n = strlen(rx) < len ? strlen(rx) : len;

	memcpy(buf, rx, n);
	return (long)n;
}

static long uart_write(void *priv, const char *buf, unsigned long len)
{
	struct uart_state *u = priv;

	printf("  uart: TX %lu bytes: %.*s\n", len, (int)len, buf);
	u->tx_count += len;
	return (long)len;
}

static void uart_release(void *priv)
{
	struct uart_state *u = priv;

	printf("  uart: closed after %u bytes sent\n", u->tx_count);
}

/* Kernel convention: the ops table is static const — one shared, read-only
 * instance for all devices of this type. */
static const struct dev_ops uart_ops = {
	.open    = uart_open,
	.read    = uart_read,
	.write   = uart_write,
	.release = uart_release,
};

/* ---- Implementation 2: a fake flash chip ------------------------------- */

struct flash_state {
	char data[64];
	int  write_protected;
};

static int flash_open(void *priv)
{
	(void)priv;
	printf("  flash: JEDEC id read OK\n");
	return 0;
}

static long flash_read(void *priv, char *buf, unsigned long len)
{
	struct flash_state *f = priv;
	unsigned long n = sizeof(f->data) < len ? sizeof(f->data) : len;

	memcpy(buf, f->data, n);
	return (long)n;
}

static long flash_write(void *priv, const char *buf, unsigned long len)
{
	struct flash_state *f = priv;

	if (f->write_protected) {
		printf("  flash: write rejected (WP# asserted)\n");
		return -1; /* kernel would return -EROFS */
	}
	if (len > sizeof(f->data))
		len = sizeof(f->data);
	memcpy(f->data, buf, len);
	return (long)len;
}

static void flash_release(void *priv)
{
	(void)priv;
	printf("  flash: deselected\n");
}

static const struct dev_ops flash_ops = {
	.open    = flash_open,
	.read    = flash_read,
	.write   = flash_write,
	.release = flash_release,
};

/* ---- The "subsystem": generic code that knows nothing about UARTs or
 *      flash chips, only about struct dev_ops. ---------------------------- */

static void exercise_device(struct device *dev)
{
	char buf[16] = {0};
	long n;

	printf("driving '%s' through its ops table:\n", dev->name);
	if (dev->ops->open(dev->priv) != 0)
		return;

	dev->ops->write(dev->priv, "ping", 4);

	n = dev->ops->read(dev->priv, buf, sizeof(buf) - 1);
	if (n >= 0)
		printf("  read %ld bytes: \"%.*s\"\n", n, (int)n, buf);

	dev->ops->release(dev->priv);
}

int main(void)
{
	struct uart_state  uart  = { .baud = 115200 };
	struct flash_state flash = { .data = "old flash contents",
				     .write_protected = 1 };

	struct device devices[] = {
		{ .name = "ttyFAKE0", .ops = &uart_ops,  .priv = &uart  },
		{ .name = "mtdFAKE0", .ops = &flash_ops, .priv = &flash },
	};

	for (unsigned i = 0; i < sizeof(devices) / sizeof(devices[0]); i++)
		exercise_device(&devices[i]);

	return 0;
}
