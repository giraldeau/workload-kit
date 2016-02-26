obj-m += wk_lock.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
modules_install:
	make -C $(KDIR) M=$(PWD) modules_install
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

