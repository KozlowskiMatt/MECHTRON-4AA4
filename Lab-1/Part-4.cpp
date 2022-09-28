#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#define AUTHOR "Eric and Matt"
#define DESC "4AA4 Lab2 Part3"
#define MODULE

static char *my_string="dummy";
int my_int = 0;

module_param(my_string, charp,0000);
MODULE_PARM_DESC(my_string,"A character string");

module_param(my_int, int, 0000);
MODULE_PARM_DESC(my_int,"An integer");

static int __init init_custom_module(void)
{
  printk(KERN_INFO  "Hello dummy world %s\n", my_name);
	printk(KERN_INFO "my_string = %s",my_string);
	printk(KERN_INFO "my_int = %d",my_int);
  return 0;
}

static void __exit cleanup_custom_module(void)
{
  printk(KERN_INFO "Goodbye, %s\n", my_name);
}

module_init(init_custom_module);
module_exit(cleanup_custom_module);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
