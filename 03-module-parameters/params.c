// SPDX-License-Identifier: GPL-2.0
/*
 * params.c — module parameters and their sysfs representation.
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>

static int rate = 100;
module_param(rate, int, 0644); /* root may change it via sysfs at runtime */
MODULE_PARM_DESC(rate, "Sampling rate in Hz (default 100)");

static bool verbose; /* kernel style: rely on static zero-init, no '= false' */
module_param(verbose, bool, 0644);
MODULE_PARM_DESC(verbose, "Enable chatty logging (default N)");

static char *label = "default";
module_param(label, charp, 0444); /* read-only after load */
MODULE_PARM_DESC(label, "Instance label string");

static int thresholds[4] = { 10, 20, 30, 40 };
static int n_thresholds = 4;
module_param_array(thresholds, int, &n_thresholds, 0444);
MODULE_PARM_DESC(thresholds, "Up to 4 alarm thresholds");

static int __init params_init(void)
{
	int i;

	pr_info("params: loaded with rate=%d verbose=%d label=\"%s\"\n",
		rate, verbose, label);
	pr_info("params: %d threshold(s):", n_thresholds);
	for (i = 0; i < n_thresholds; i++)
		pr_cont(" %d", thresholds[i]);
	pr_cont("\n");

	if (verbose)
		pr_info("params: verbose mode: I will narrate everything\n");

	return 0;
}

static void __exit params_exit(void)
{
	/* If someone wrote to /sys/module/params/parameters/rate while we
	 * were loaded, this prints the NEW value — the variable simply
	 * changed underneath us. */
	pr_info("params: unloading; final rate=%d verbose=%d\n", rate, verbose);
}

module_init(params_init);
module_exit(params_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Hector Gomez");
MODULE_DESCRIPTION("Tutorial module 03: module parameters");
