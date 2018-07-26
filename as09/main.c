#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/seq_file.h>
#include <linux/mount.h>
#include <linux/proc_fs.h>
#include <linux/fs_struct.h>
#include <linux/fs.h>
#include <linux/namei.h>

MODULE_LICENSE("GPL");

static struct proc_dir_entry *proc_entry;

static int create_seq(struct vfsmount *root, void *data)
{
	//struct super_block *root_sb;
	//struct dentry *root_root;
	struct seq_file *s;
	
	s = (struct seq_file *)data;
	return (0);
}

static void mounts(char *dir, struct seq_file *s)
{
	struct path path;
	struct vfsmount *root;

	kern_path(dir, LOOKUP_FOLLOW, &path);
	root = collect_mounts(&path);
	iterate_mounts(create_seq, (void *)s, root);
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
	remove_proc_entry("mymounts", NULL);
	printk(KERN_INFO "Cleaning up module.\n");
}
module_init(entry_point);
module_exit(exit_point)
