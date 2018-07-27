#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>
#include <linux/hid.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Tshilidzi Tshivhula <pboyfunclub@gmail.com>");
MODULE_DESCRIPTION("usb module");

static int driverprobe(struct usb_interface *intf,
		const struct usb_device_id *id)
{
	printk(KERN_INFO "In the probe function\n");
	return 0;
}

static void driverdisconnect(struct usb_interface *intf)
{
	printk(KERN_INFO "A USB device disconnected\n");
}

static const struct usb_device_id supported_d_table[] = {
	{ USB_INTERFACE_INFO(USB_INTERFACE_CLASS_HID,
			     USB_INTERFACE_SUBCLASS_BOOT,
			     USB_INTERFACE_PROTOCOL_KEYBOARD) },
	{ },
};

MODULE_DEVICE_TABLE(usb, supported_d_table);

static struct usb_driver ttshivhu = {
	.name = "ttshivhu",
	.id_table = supported_d_table,
	.probe = driverprobe,
	.disconnect = driverdisconnect,
};

static int __init entry_point(void)
{
	printk(KERN_INFO "Hello world!\n");
	return usb_register(&ttshivhu);
}

static void __exit exit_point(void)
{
	printk(KERN_INFO "Cleaning up module.\n");
	usb_deregister(&ttshivhu);
}
module_init(entry_point);
module_exit(exit_point);
