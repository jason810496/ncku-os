# Virtual File System

## Test Setup

```bash
make refresh # compile and reload the kernel module

mkdir mnt # create a mount point
sudo mount -t osfs none mnt/ # mount the filesystem
sudo dmesg -wH # show the kernel log, -w to follow, -H for human readable time

cd mnt
sudo touch hello.txt # create a file
sudo bash -c 'echo "Hello, World!" > hello.txt' # write to the file
```