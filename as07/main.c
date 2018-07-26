#include <linux/module.h>
#include <linux/init.h>
#include <linux/debugfs.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/jiffies.h>
#include <linux/semaphore.h>

#define LEN 8
#define USERNAME "ttshivhu"

char kbuff[LEN], foobuff[PAGE_SIZE];
struct dentry *root;
int ret;
struct mutex flock;

MODULE_LICENSE("GPL");

static ssize_t ft_read(struct file *f, char __user *buffer, size_t length,
		       loff_t *offset)
{
	char *read_from = USERNAME + *offset;
	size_t read_num = length < (LEN - *offset) ? length : (LEN - *offset);

	if (read_num == 0)
		return (0);
	ret = copy_to_user(buffer, read_from, read_num);
	if (ret == read_num) {
		ret = -EIO;
	} else {
		*offset = LEN - ret;
		ret = read_num - ret;
	}
	return ret;
}

static ssize_t ft_write(struct file *f, const char __user *buf, size_t len,
			loff_t *offset)
{
	if (len != LEN) {
		ret = -EINVAL;
		return (ret);
	}
	ret = copy_from_user(kbuff, buf, LEN);
	if (strncmp(kbuff, USERNAME, LEN) == 0)
		ret = LEN;
	else
		ret = -EINVAL;
	return ret;
}

static ssize_t foor(struct file *f, char __user *buffer, size_t length,
		    loff_t *offset)
{
	char *read_from = foobuff + *offset;
	size_t read_num = length < (PAGE_SIZE - *offset) ?
		length : (PAGE_SIZE - *offset);

	ret = mutex_lock_interruptible(&flock);
	if (ret)
		return -1;
	if (read_num == 0) {
		ret = 0;
		goto cleanup;
	}
	ret = copy_to_user(buffer, read_from, read_num);
	if (ret == read_num) {
		ret = -EIO;
	} else {
		*offset = PAGE_SIZE - ret;
		ret = read_num - ret;
	}
cleanup:
	mutex_unlock(&flock);
	return ret;
}

static ssize_t foow(struct file *f, const char __user *buf, size_t len,
		    loff_t *offset)
{
	int bytes_write = 0;
	int append = 0;

	ret = mutex_lock_interruptible(&flock);
	if (ret)
		return -1;
	if (f->f_flags & O_APPEND)
		append = strlen(foobuff);
	if (*offset + append >= PAGE_SIZE)
		ret =  -EINVAL;
	while ((bytes_write < len) && (*offset + append < PAGE_SIZE)) {
		get_user(foobuff[append + *offset], &buf[bytes_write]);
		*offset = *offset + 1;
		bytes_write++;
	}
	mutex_unlock(&flock);
	return bytes_write ? bytes_write : ret;
}

static struct file_operations idfops = {
  .owner = THIS_MODULE,
  .read = ft_read,
  .write = ft_write,
};

static struct file_operations foofops = {
  .owner = THIS_MODULE,
  .read = foor,
  .write = foow,
};

static int __init entry_point(void)
{
	printk(KERN_INFO "Hello world!\n");
	root = debugfs_create_dir("fortytwo", NULL);
	if (!root || root == (void *)-ENODEV)
		return -1;
	if (!(debugfs_create_file("id", 0666, root, NULL, &idfops) &&
			debugfs_create_ulong("jiffies", 0444, root,
			   (long unsigned int *)&jiffies) &&
			debugfs_create_file("foo", 0644, root, NULL,
				&foofops)))
		return -1;
	mutex_init(&flock);
	return 0;
}

static void __exit exit_point(void)
{
	debugfs_remove_recursive(root);
	printk(KERN_INFO "Cleaning up module.\n");
}
module_init(entry_point);
module_exit(exit_point);
