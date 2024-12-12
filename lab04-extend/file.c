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

    pr_info("osfs_read: Reading %zu bytes from file with inode %lu\n", len, inode->i_ino);
    pr_info("osfs_read: osfs_inode->i_ino: %u, osfs_inode->i_size: %u, osfs_inode->i_blocks: , osfs_inode->extent_count: %u\n", osfs_inode->i_ino, osfs_inode->i_size, osfs_inode->i_blocks, osfs_inode->extent_count);
    pr_info("osfs_read: ppos: %lld\n", *ppos);

    // If the file has not been allocated a data block, it indicates the file is empty
    if (osfs_inode->extent_count == 0){
        return 0;
    }

    if (*ppos >= osfs_inode->i_size){
        return 0;
    }
    int bytes_to_read = osfs_inode->i_size - *ppos;
    pr_info("osfs_read: bytes_to_read: %d\n", bytes_to_read);

    // data_block = sb_info->data_blocks + osfs_inode->i_blocks * BLOCK_SIZE + *ppos;
    // if (copy_to_user(buf, data_block, len))
    //     return -EFAULT;
    struct osfs_extent *current_extent = osfs_inode->extents;
    while(current_extent != NULL ){
        data_block = sb_info->data_blocks + current_extent->block_no * BLOCK_SIZE + *ppos;
        int current_block_size = bytes_to_read > BLOCK_SIZE ? BLOCK_SIZE : bytes_to_read;
        if (copy_to_user(buf, data_block, current_block_size)) {
            pr_err("osfs_read: Failed to copy data to user space\n");
            return -EFAULT;
        }
        pr_info("osfs_read: Reading %d bytes from block %u\n", current_block_size, current_extent->block_no);
        bytes_to_read -= current_block_size;
        buf += current_block_size;
        bytes_read += current_block_size;
        *ppos += current_block_size;
        current_extent = current_extent->next;
    }

    pr_info("osfs_read: Read %zd bytes from file with inode %lu\n", bytes_read, inode->i_ino);
    pr_info("osfs_read: bytes_to_read: %d\n", bytes_to_read);
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
    int need_block_count = len / BLOCK_SIZE + (len % BLOCK_SIZE != 0);
    
    if(*ppos == 0 ){
        osfs_inode->i_size = len;
    }

    pr_info("osfs_write: ------------------------\n");
    pr_info("osfs_write: Writing %zu bytes to file with inode %lu\n", len, inode->i_ino);
    pr_info("osfs_write: osfs_inode->i_ino: %u, osfs_inode->i_size: %u, osfs_inode->i_blocks: %u\n", osfs_inode->i_ino, osfs_inode->i_size, osfs_inode->i_blocks);
    pr_info("osfs_write: len: %zu\n", len);
    pr_info("osfs_write: ppos: %lld\n", *ppos);
    pr_info("osfs_write: need_block_count: %d\n", need_block_count);

    pr_info("osfs_write: need_block_count: %d\n", need_block_count);

    struct osfs_extent *current_extent = osfs_inode->extents;
    struct osfs_extent *prev_extent = NULL;
    while(need_block_count--){
        if(current_extent == NULL){
            current_extent = kmalloc(sizeof(struct osfs_extent), GFP_KERNEL);
            pr_info("osfs_write: Allocating new extent\n");
            if(!current_extent){
                pr_err("osfs_write: Failed to allocate memory for new extent\n");
                return -ENOMEM;
            }
            if( osfs_alloc_data_block(sb_info, &(current_extent->block_no)) !=0  ){
                pr_err("osfs_write: Failed to allocate data block for new extent\n");
                return -ENOSPC;
            }
            if(current_extent->block_no < 0){
                pr_err("osfs_write: Failed to allocate data block for new extent\n");
                return current_extent->block_no;
            }
            osfs_inode->extent_count++;
            osfs_inode->i_blocks++;
            current_extent->next = NULL;
            if(osfs_inode->extents == NULL){
                osfs_inode->extents = current_extent;
            } else {
                prev_extent->next = current_extent;
            }
        }
        // copy data to data block
        data_block = sb_info->data_blocks + current_extent->block_no * BLOCK_SIZE + *ppos;
        int current_block_size = len > BLOCK_SIZE ? BLOCK_SIZE : len;
        if (copy_from_user(data_block, buf, current_block_size)) {
            pr_err("osfs_write: Failed to copy data from user space\n");
            return -EFAULT;
        }
        len -= current_block_size;
        buf += current_block_size;
        bytes_written += current_block_size;
        *ppos += current_block_size;

        pr_info("osfs_write: Writing %d bytes to block %u\n", current_block_size, current_extent->block_no);

        prev_extent = current_extent;
        current_extent = current_extent->next;
    }

    // Step5: Update inode & osfs_inode attribute
    mark_inode_dirty(inode);

    pr_info("osfs_write: Debug extents list\n");
    // loop through extents
    int ith_extent = 0;
    current_extent = osfs_inode->extents;
    while(current_extent){
        pr_info("osfs_write: extents[%d]: block_no: %u\n", ith_extent, current_extent->block_no);
        current_extent = current_extent->next;
        ith_extent++;
    }

    // Step6: Return the number of bytes written

    pr_info("osfs_write: osfs_inode->i_size: %u, osfs_inode->i_blocks: %u\n", osfs_inode->i_size, osfs_inode->i_blocks);
    pr_info("osfs_write: bytes_written: %zu\n", bytes_written);
    pr_info("osfs_write: ppos: %lld\n", *ppos);
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
