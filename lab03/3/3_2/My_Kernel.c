#include <linux/kernel.h>
#include <linux/atomic.h> 

#include <linux/module.h>
#include <linux/init.h>
#include <linux/printk.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <asm/current.h>

#define procfs_name "Mythread_info"
#define BUFSIZE  1024
char buf[BUFSIZE]; //kernel buffer


static unsigned long procfs_buffer_size = 0; 
static unsigned long last_read_pos = -1;
enum { 
    CDEV_NOT_USED, 
    CDEV_EXCLUSIVE_OPEN, 
}; 
static atomic_t already_open = ATOMIC_INIT(CDEV_NOT_USED);


// // Function to handle opening of the proc file
// static int device_open(struct inode *inode, struct file *file) {
//     if (atomic_cmpxchg(&already_open, CDEV_NOT_USED, CDEV_EXCLUSIVE_OPEN)) 
//         return -EBUSY; 

//     pr_info("My_Kernel: Device opened %p\n", file);

//     return 0;
// }

// // Function to handle closing of the proc file
// static int device_release(struct inode *inode, struct file *file) {
//     atomic_set(&already_open, CDEV_NOT_USED); 
//     pr_info("My_Kernel: Device closed %p\n", file);
//     return 0;
// }

static ssize_t Mywrite(struct file *fileptr, const char __user *ubuf, size_t buffer_len, loff_t *offset){
    /*Your code here*/

    if(buffer_len > BUFSIZE){
        pr_info("My_Kernel: Input size is too large\n");
        return -ENOSPC;
    }

    procfs_buffer_size = buffer_len;
    if (procfs_buffer_size >= BUFSIZE){
        procfs_buffer_size = BUFSIZE - 1;
    }

    if (copy_from_user(buf, ubuf, buffer_len)){
        return -EFAULT;
    }

    struct task_struct *p = current;
    procfs_buffer_size += sprintf(buf + procfs_buffer_size, "PID: %d, TID: %d, Priority: %d, State: %ld\n", 
        p->tgid, p->pid, p->prio, p->__state);

    buf[procfs_buffer_size] = '\0';
    *offset += procfs_buffer_size;

    pr_info("My_Kernel: Received %lu characters from the user\n", procfs_buffer_size);
    pr_info("My_Kernel: Data from the user: %s\n", buf);

    return procfs_buffer_size;

    /****************/
}


static ssize_t Myread(struct file *fileptr, char __user *ubuf, size_t buffer_len, loff_t *offset){
    /*Your code here*/
    if(*offset > 0 ){
        // only one read
        return 0;
    }

    ssize_t len = procfs_buffer_size;
    if (len > buffer_len){
        len = buffer_len;
    }

    if (copy_to_user(ubuf, buf, len)){
        return -EFAULT;
    }

    *offset += len;
    return len;

    /****************/
}

static struct proc_ops Myops = {
    .proc_read = Myread,
    .proc_write = Mywrite,
};


static int My_Kernel_Init(void){
    proc_create(procfs_name, 0644, NULL, &Myops);   
    pr_info("My kernel says Hi");
    // register device
    return 0;
}

static void My_Kernel_Exit(void){
    pr_info("My kernel says GOODBYE");
    remove_proc_entry(procfs_name, NULL);
    pr_info("My_Kernel: Module unloaded\n");

    // unregister device
}

module_init(My_Kernel_Init);
module_exit(My_Kernel_Exit);

MODULE_LICENSE("GPL");
