// SPDX-License-Identifier: GPL-2.0
/*
 * hello.c — the canonical first kernel module.
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>   /* current (the running task) */
#include <linux/utsname.h> /* init_uts_ns */

static int __init hello_init(void)
{
	pr_info("hello: loaded into kernel %s\n", init_uts_ns.name.release);
	pr_info("hello: init runs in process '%s' (pid %d)\n",
		current->comm, current->pid);
	return 0; /* nonzero would abort the load */
}

static void __exit hello_exit(void)
{
	pr_info("hello: unloaded, goodbye\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Hector Gomez");
MODULE_DESCRIPTION("Tutorial module 02: hello world");
MODULE_VERSION("1.0");
