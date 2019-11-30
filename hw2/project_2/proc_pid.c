/**
 * proc_pid.c
 *
 * Kernel module for task information.
 *
 * Operating System Concepts - 10th Edition
 * Copyright John Wiley & Sons - 2018
 */

#include <linux/init.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

#define BUFFER_SIZE 128
#define PROC_NAME "pid"

static int current_pid;

static ssize_t proc_read(struct file *file, char *buf, size_t count, loff_t *pos);
static ssize_t proc_write(struct file *file, const char __user *usr_buf, size_t count, loff_t *pos);

static struct file_operations proc_ops = {
    .owner = THIS_MODULE,
    .read = proc_read,
    .write = proc_write
};

/* This function is called when the module is loaded. */
static int proc_init(void)
{
    /** creates the /proc/pid entry
        the right is also set **/
    proc_create(PROC_NAME, 0666, NULL, &proc_ops);

    printk(KERN_INFO "/proc/%s created\n", PROC_NAME);

    return 0;
}

/* This function is called when the module is removed. */
static void proc_exit(void) 
{
    /* removes the /proc/procfs entry */
    remove_proc_entry(PROC_NAME, NULL);

    printk(KERN_INFO "/proc/%s removed\n", PROC_NAME);
}

/**
 * This function is called each time the /proc/pid is read.
 * 
 * This function is called repeatedly until it returns 0, so
 * there must be logic that ensures it ultimately returns 0
 * once it has collected the data that is to go into the 
 * corresponding /proc file.
 */
static ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos)
{
    char buffer[BUFFER_SIZE];
    int len = 0;
    struct task_struct *tsk;

    if (*pos > 0 || count < BUFFER_SIZE) {
        return 0;
    }

    tsk = pid_task(find_vpid(current_pid), PIDTYPE_PID);
    
    if (tsk) {
        len += sprintf(buffer, "command = [%s], pid = [%d], state = [%ld]\n",
                 tsk->comm, current_pid, tsk->state);
    }

    else {
        printk(KERN_INFO "Invalid PID %d written to /proc/pid\n", current_pid);
        return 0;
    }

    /* copies the contents of kernel buffer to userspace usr_buf */ 
    if (copy_to_user(usr_buf, buffer, len)) {
        printk(KERN_INFO "Error occurs when copying from user\n");
        return -1;
    }

    /* updates the position and returns the number of bytes we received */
	*pos = len;
	return len;
}

/**
 * This function is called each time we write to the /proc file system.
 */
static ssize_t proc_write(struct file *file, const char __user *usr_buf, size_t count, loff_t *pos)
{
    char *k_mem;
    long pid;

    /* allocates kernel memory */
    k_mem = kmalloc(count + 1, GFP_KERNEL);

    /* copies user space usr_buf to kernel buffer */
    if (copy_from_user(k_mem, usr_buf, count)) {
        printk(KERN_INFO "Error occurs when copying from user\n");
        return -1;
    }

    k_mem[count] = '\0';

    /* obtains the long integer */
    kstrtol(k_mem, 10, &pid);
    current_pid = (int) pid;

    /* return kernel memory */
    kfree(k_mem);

    return count;
}

/* Macros for registering module entry and exit points. */
module_init( proc_init );
module_exit( proc_exit );

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Pid Module");
MODULE_AUTHOR("SGG");