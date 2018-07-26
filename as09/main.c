#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/namei.h>
#include <linux/list.h>
#include <linux/dcache.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

MODULE_LICENSE("GPL");

static struct proc_dir_entry *proc_entry;

static void mounts(const char *dir, struct seq_file *s)
{
    struct path path;
    struct dentry *thedentry;
    struct dentry *curdentry;

    kern_path(dir, LOOKUP_FOLLOW, &path);
    thedentry = path.dentry;
    list_for_each_entry(curdentry, &thedentry->d_subdirs, d_child) {
        if ( curdentry->d_flags & DCACHE_MOUNTED) {
		seq_printf(s, "%s /%s\n", curdentry->d_name.name, dir);
	}
    }
}

static int seq_mounts(struct seq_file *s, void *v)
{
	mounts("/", s);
	return 0;
}

static int opened(struct inode *i, struct file *f)
{
	return single_open(f, &seq_mounts, NULL);
}

static struct file_operations seqfops = {
  .owner = THIS_MODULE,
  .open = opened,
  .read = seq_read,
};


static int __init entry_point(void)
{
	printk(KERN_INFO "Hello world!\n");
	proc_entry = proc_create("mymounts", 0644, NULL, &seqfops);
	return 0;
}

static void __exit exit_point(void)
{
	remove_proc_entry("mymounts", proc_entry);
	printk(KERN_INFO "Cleaning up module.\n");
}
module_init(entry_point);
module_exit(exit_point)
