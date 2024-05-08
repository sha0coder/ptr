obj-m += ptr_drv.o

KERNEL_DIR ?= /lib/modules/$(shell uname -r)/build

ccflags-y := -I$(PWD)/include

ptr_drv-objs := driver/ptr_comm.o driver/ptr_drv.o driver/ptr_modules.o

all:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) clean

install:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) modules_install
	depmod -a
