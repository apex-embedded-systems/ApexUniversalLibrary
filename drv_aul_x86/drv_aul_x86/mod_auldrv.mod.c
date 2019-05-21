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
	{ 0xd4733cff, "module_layout" },
	{ 0x1fedf0f4, "__request_region" },
	{ 0xf550b75d, "cdev_del" },
	{ 0x12da5bb2, "__kmalloc" },
	{ 0xb9c78ada, "cdev_init" },
	{ 0x3ec8886f, "param_ops_int" },
	{ 0xd8e484f0, "register_chrdev_region" },
	{ 0x5792fc3, "device_destroy" },
	{ 0x7485e15e, "unregister_chrdev_region" },
	{ 0x72aa82c6, "param_ops_charp" },
	{ 0xff7559e4, "ioport_resource" },
	{ 0x50eedeb8, "printk" },
	{ 0x96661662, "class_unregister" },
	{ 0x2da418b5, "copy_to_user" },
	{ 0xb6ed1e53, "strncpy" },
	{ 0x66355efc, "vprintk" },
	{ 0x71abefe1, "device_create" },
	{ 0xf6ede60b, "cdev_add" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0x7c61340c, "__release_region" },
	{ 0x37a0cba, "kfree" },
	{ 0xf59f197, "param_array_ops" },
	{ 0xd13b2b18, "class_destroy" },
	{ 0x33d169c9, "_copy_from_user" },
	{ 0xc3fe87c8, "param_ops_uint" },
	{ 0x1df70be4, "__class_create" },
	{ 0x29537c9e, "alloc_chrdev_region" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

