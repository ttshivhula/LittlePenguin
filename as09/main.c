#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/namei.h>
#include <linux/list.h>
#include <linux/dcache.h>
#include <linux/proc_fs.h>

MODULE_LICENSE("GPL");

static struct proc_dir_entry *proc_entry;

static struct file_operations devfops = {
  .owner = THIS_MODULE,
  .read = ft_read,
};

static void mounts(const char *dir)
{
    struct path path;
    struct dentry *thedentry;
    struct dentry *curdentry;

    kern_path(dir, LOOKUP_FOLLOW, &path);
    thedentry = path.dentry;
    list_for_each_entry(curdentry, &thedentry->d_subdirs, d_child) {
        if ( curdentry->d_flags & DCACHE_MOUNTED) {
		printk("%s is mounted", curdentry->d_name.name);
		//mounts(curdentry->d_name.name);
	}
    }
}

static int __init entry_point(void)
{
	printk(KERN_INFO "Hello world!\n");
	//mounts("/");
	proc_entry = proc_create("fortune", 0644, NULL, NULL);
	return 0;
}

static void __exit exit_point(void)
{
	//remove_proc_entry("fortune", proc_entry);
	printk(KERN_INFO "Cleaning up module.\n");
}
module_init(entry_point);
module_exit(exit_point)
