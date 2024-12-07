KDIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
TARGET_MODULE := osfs

obj-m += osfs.o

osfs-objs := super.o inode.o file.o dir.o osfs_init.o

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

load:
	@sudo insmod $(TARGET_MODULE).ko

unload:
	@sudo rmmod $(TARGET_MODULE).ko

refresh:
	@if lsmod | grep -q $(TARGET_MODULE); then \
		sudo rmmod $(TARGET_MODULE); \
	fi
	@make clean
	@make all
	@make load

unmount:
	@sudo umount mnt
	@rm -rf mnt

mount:
	@mkdir mnt
	@sudo mount -t osfs none mnt/

test:
	@cd mnt && sudo touch hello.txt
	@cd mnt && sudo bash -c 'echo "Hello, World!" > hello.txt'