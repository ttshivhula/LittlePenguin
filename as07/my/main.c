#include <linux/module.h>
#include <linux/init.h>
#include <linux/debugfs.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/uaccess.h>

#define LEN 8
#define USERNAME "ttshivhu"

char kbuff[LEN];

MODULE_LICENSE("GPL");

struct dentry *root;
int ret;


static ssize_t ft_read(struct file *f, char __user *buffer, size_t length, loff_t *offset)
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

static ssize_t ft_write(struct file *f, const char __user *buf, size_t len, loff_t *offset)
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
	return (ret);
}

static struct file_operations idfops = {
  .owner = THIS_MODULE,
  .read = ft_read,
  .write = ft_write,
  .llseek = no_llseek,
};

#include <linux/jiffies.h>

// delay between two calls of my_timer_function in jiffies
static const int delay_jiffies = 1 * HZ;

static int __init entry_point(void)
{
	root = debugfs_create_dir("fortytwo", NULL);
	if (!root || root == (void *)-ENODEV)
		return (-1);
	debugfs_create_file("id", 0666, root, NULL, &idfops);
	debugfs_create_u8("jiffies", 0444, root, (u8 *)&delay_jiffies);
	debugfs_create_file("foo", 0644, root, NULL, &idfops);
	printk(KERN_INFO "Hello world! %d\n", delay_jiffies);
	return 0;
}

static void __exit exit_point(void)
{
	debugfs_remove_recursive(root);
	printk(KERN_INFO "Cleaning up module.\n");
}
module_init(entry_point);
module_exit(exit_point);
