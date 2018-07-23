#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");

static int __init entry_point(void)
{
	printk(KERN_INFO "Hello world!\n");
	return 0;
}

static void __exit exit_point(void)
{
	printk(KERN_INFO "Cleaning up module.\n");
}
module_init(entry_point);
module_exit(exit_point)
