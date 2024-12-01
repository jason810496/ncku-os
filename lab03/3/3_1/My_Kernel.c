#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/printk.h>
#include <linux/proc_fs.h>
#include <asm/current.h>

#define procfs_name "Mythread_info"
#define target_name "3_1.out"
#define BUFSIZE  1024
char buf[BUFSIZE];

static ssize_t Mywrite(struct file *fileptr, const char __user *ubuf, size_t buffer_len, loff_t *offset){
    /* Do nothing */
	return 0;
}


static ssize_t Myread(struct file *fileptr, char __user *ubuf, size_t buffer_len, loff_t *offset){
    // get all thread information from current process
    // for each thread, 
    // print PID, TID, Priority, State

    struct task_struct *task_child;
    struct list_head *list;
    int len = 0;
    char *bufptr = buf;

    // Clear the buffer
    memset(buf, 0, BUFSIZE);

    len += sprintf(bufptr, "PID: %d\n", current->pid);
    len += sprintf(bufptr + len, "TID: %d\n", current->tgid);
    len += sprintf(bufptr + len, "Priority: %d\n", current->prio);
    len += sprintf(bufptr + len, "State: %c\n", task_state_to_char(current));
    list_for_each(list, &current->children){
        task_child = list_entry(list, struct task_struct, sibling);
        len += sprintf(bufptr + len, "PID: %d\n", task_child->pid);
        len += sprintf(bufptr + len, "TID: %d\n", task_child->tgid);
        len += sprintf(bufptr + len, "Priority: %d\n", task_child->prio);
        len += sprintf(bufptr + len, "State: %c\n", task_state_to_char(task_child));

        // Stop writing if the buffer is full
        if (len >= BUFSIZE) {
            break;
        }
    }

    // Check if the user buffer is large enough to hold the data
    if (*offset >= len) {
        return 0; // End of file
    }
    if (buffer_len < len - *offset) {
        return -EINVAL; // Buffer too small
    }

    // Copy data to the user buffer
    if (copy_to_user(ubuf, buf + *offset, len - *offset)) {
        return -EFAULT; // Failed to copy data
    }

    // Update the offset and return the number of bytes read
    *offset += len;
    return len - *offset;
}

static struct proc_ops Myops = {
    .proc_read = Myread,
    .proc_write = Mywrite,
};

static int My_Kernel_Init(void){
    proc_create(procfs_name, 0644, NULL, &Myops);   
    pr_info("My kernel says Hi");
    return 0;
}

static void My_Kernel_Exit(void){
    pr_info("My kernel says GOODBYE");
}

module_init(My_Kernel_Init);
module_exit(My_Kernel_Exit);

MODULE_LICENSE("GPL");