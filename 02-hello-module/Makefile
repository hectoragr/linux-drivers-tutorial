# Out-of-tree module Makefile. This file is read TWICE:
#
#  1. By you running `make` here: KERNELRELEASE is unset, so the lower
#     block runs and re-invokes make inside the kernel's build tree.
#  2. By kbuild (that inner make): KERNELRELEASE is set, so only the
#     obj-m line matters.

ifneq ($(KERNELRELEASE),)
# --- kbuild part ---
obj-m := hello.o

else
# --- normal-make part ---
KDIR ?= /lib/modules/$(shell uname -r)/build

default:
	$(MAKE) -C $(KDIR) M=$(CURDIR) modules

clean:
	$(MAKE) -C $(KDIR) M=$(CURDIR) clean

endif
