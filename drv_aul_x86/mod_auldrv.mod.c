#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xeb13df47, "module_layout" },
	{ 0x1fedf0f4, "__request_region" },
	{ 0xfd9a81c0, "cdev_del" },
	{ 0xd2b09ce5, "__kmalloc" },
	{ 0x8db157a6, "cdev_init" },
	{ 0x15692c87, "param_ops_int" },
	{ 0xd8e484f0, "register_chrdev_region" },
	{ 0xe745f62b, "device_destroy" },
	{ 0x7485e15e, "unregister_chrdev_region" },
	{ 0x4f8b5ddb, "_copy_to_user" },
	{ 0x35b6b772, "param_ops_charp" },
	{ 0xff7559e4, "ioport_resource" },
	{ 0x27e1a049, "printk" },
	{ 0x75ab2578, "class_unregister" },
	{ 0xa1c76e0a, "_cond_resched" },
	{ 0x9166fada, "strncpy" },
	{ 0xbf8ba54a, "vprintk" },
	{ 0x5ec209c9, "device_create" },
	{ 0x8256616, "cdev_add" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0x7c61340c, "__release_region" },
	{ 0x37a0cba, "kfree" },
	{ 0x4845c423, "param_array_ops" },
	{ 0xc40fae79, "class_destroy" },
	{ 0x4f6b400b, "_copy_from_user" },
	{ 0x6d044c26, "param_ops_uint" },
	{ 0xd7b54629, "__class_create" },
	{ 0x29537c9e, "alloc_chrdev_region" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "2C4062CE49D96903131CDBF");
