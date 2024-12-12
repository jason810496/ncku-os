#include <linux/fs.h>
#include <linux/uaccess.h>
#include "osfs.h"

/**
 * Function: osfs_read
 * Description: Reads data from a file.
 * Inputs:
 *   - filp: The file pointer representing the file to read from.
 *   - buf: The user-space buffer to copy the data into.
 *   - len: The number of bytes to read.
 *   - ppos: The file position pointer.
 * Returns:
 *   - The number of bytes read on success.
 *   - 0 if the end of the file is reached.
 *   - -EFAULT if copying data to user space fails.
 */
static ssize_t osfs_read(struct file *filp, char __user *buf, size_t len, loff_t *ppos)
{
    struct inode *inode = file_inode(filp);
    struct osfs_inode *osfs_inode = inode->i_private;
    struct osfs_sb_info *sb_info = inode->i_sb->s_fs_info;
    void *data_block;
    ssize_t bytes_read;

    // If the file has not been allocated a data block, it indicates the file is empty
    if (osfs_inode->i_blocks == 0)
        return 0;

    if (*ppos >= osfs_inode->i_size)
        return 0;

    if (*ppos + len > osfs_inode->i_size)
        len = osfs_inode->i_size - *ppos;

    data_block = sb_info->data_blocks + osfs_inode->i_blocks * BLOCK_SIZE + *ppos;
    if (copy_to_user(buf, data_block, len))
        return -EFAULT;

    *ppos += len;
    bytes_read = len;

    return bytes_read;
}


/**
 * Function: osfs_write
 * Description: Writes data to a file.
 * Inputs:
 *   - filp: The file pointer representing the file to write to.
 *   - buf: The user-space buffer containing the data to write.
 *   - len: The number of bytes to write.
 *   - ppos: The file position pointer.
 * Returns:
 *   - The number of bytes written on success.
 *   - -EFAULT if copying data from user space fails.
 *   - Adjusted length if the write exceeds the block size.
 */
static ssize_t osfs_write(struct file *filp, const char __user *buf, size_t len, loff_t *ppos)
{   
    //Step1: Retrieve the inode and filesystem information
    struct inode *inode = file_inode(filp);
    struct osfs_inode *osfs_inode = inode->i_private;
    struct osfs_sb_info *sb_info = inode->i_sb->s_fs_info;
    void *data_block;
    ssize_t bytes_written;
    int ret;

    if(*ppos == 0 ){
        osfs_inode->i_size = len;
        inode->i_size = len;
    }

    pr_info("osfs_write: Writing %zu bytes to file with inode %lu\n", len, inode->i_ino);
    pr_info("osfs_write: osfs_inode->i_ino: %u, osfs_inode->i_size: %u, osfs_inode->i_blocks: %u\n", osfs_inode->i_ino, osfs_inode->i_size, osfs_inode->i_blocks);
    pr_info("osfs_write: ppos: %lld\n", *ppos);

    // Step2: Check if a data block has been allocated; if not, allocate one
    if (osfs_inode->i_blocks == 0) {
        pr_info("osfs_write: Allocating data block for inode %u\n", osfs_inode->i_ino);
        osfs_inode->i_block = osfs_alloc_data_block(sb_info, &osfs_inode->i_block);
        if (osfs_inode->i_block < 0)
            return osfs_inode->i_block;
        osfs_inode->i_blocks = 1;
    }

    // Step3: Limit the write length to fit within one data block
    if (*ppos + len > BLOCK_SIZE) {
        len = BLOCK_SIZE - *ppos;
    }
    pr_info("osfs_write: Adjusted length: %zu\n", len);


    // Step4: Write data from user space to the data block
    data_block = sb_info->data_blocks + osfs_inode->i_blocks * BLOCK_SIZE + *ppos;
    if (copy_from_user(data_block, buf, len)) {
        pr_err("osfs_write: Failed to copy data from user space\n");
        return -EFAULT;
    }


    // Step5: Update inode & osfs_inode attribute
    osfs_inode->i_size = *ppos + len;
    inode->i_size = osfs_inode->i_size;
    osfs_inode->__i_atime = osfs_inode->__i_mtime = current_time(inode);
    inode->__i_atime = inode->__i_mtime = osfs_inode->__i_mtime;
    mark_inode_dirty(inode);
    *ppos += len;
    bytes_written = len;


    // Step6: Return the number of bytes written

    pr_info("osfs_write: osfs_inode->i_size: %u, osfs_inode->i_blocks: %u\n", osfs_inode->i_size, osfs_inode->i_blocks);
    pr_info("osfs_write: bytes_written: %zu\n", bytes_written);
    return bytes_written;
}

/**
 * Struct: osfs_file_operations
 * Description: Defines the file operations for regular files in osfs.
 */
const struct file_operations osfs_file_operations = {
    .open = generic_file_open, // Use generic open or implement osfs_open if needed
    .read = osfs_read,
    .write = osfs_write,
    .llseek = default_llseek,
    // Add other operations as needed
};

/**
 * Struct: osfs_file_inode_operations
 * Description: Defines the inode operations for regular files in osfs.
 * Note: Add additional operations such as getattr as needed.
 */
const struct inode_operations osfs_file_inode_operations = {
    // Add inode operations here, e.g., .getattr = osfs_getattr,
};
