#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h> //module_init and module_exit are defined here

static int foo __initdata = 2;

static int __init init_custom_module(void)
{
  printk(KERN_INFO  "Hello dummy world %d\n", foo);
  return 0;
}

static void __exit cleanup_custom_module(void)
{
  printk(KERN_INFO "<1>Hello cruel world2\n");
}

module_init(init_custom_module);    //call this function when module is inserted
module_exit(cleanup_custom_module); //call this function when module is removed
