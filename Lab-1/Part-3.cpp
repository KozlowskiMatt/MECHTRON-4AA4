#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#define AUTHOR "Matt and Eric"
#define DESC "4AA4 Lab2 Part3"
#define MODULE

static int x __initdata = 2;

static int __init init_custom_module(void)
{
  printk(KERN_INFO  "Hello dummy world %d\n", x);
  return 0;
}

static void __exit cleanup_custom_module(void)
{
  printk(KERN_INFO "<1>Hello cruel world2\n");
}

module_init(init_custom_module);
module_exit(cleanup_custom_module);

MODULE_LICENSE("GPL"); //set GPL so this code is wanred to be "non open source"
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
