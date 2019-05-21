#
#
# AUL driver kernel module Makefile using I/O simulation (array of memory)
# Apex Embedded Systems
#
# SVN Revision: "$Date: 2014-04-29 09:54:14 -0500 (Tue, 29 Apr 2014) $"


###########################################################
# Uncomment to post printk() debug messages
DEBUG_ENABLE  := -D _DEBUG

###########################################################
# Uncomment to simulate I/O activities as printk() messages
#NOT IMPLEMENTED: SIMULATE_IO   := -D IO_SIMULATION 

###########################################################
# The name of the driver and prefix name of the devices.
#   Devices will be named <NAME><device #>. Example: summit0
#   Driver module will be named mod_<NAME>. Example: mod_summit.ko
#
NAME      := test
DRV_NAME  := 'DRIVER_NAME="$(NAME)"'

###########################################################
###########################################################
###########################################################

OBJS  = \
        ../aul_tree/str_safe/str_safe.o    \
        ../aul_tree/shared/debug.o         \
        ../aul_tree/shared/error.o         \
        ../aul_tree/shared/status.o        \
        ../aul_tree/shared/mem.o           \
        ../aul_tree/io/io.o                \
        ../aul_tree/io/io_linux_lib_sim.o  \
         drv_test.o

HDR   = ../aul_tree/str_safe/str_safe.h  \
        ../aul_tree/shared/debug.h       \
        ../aul_tree/shared/error.h       \
        ../aul_tree/shared/status.h      \
        ../aul_tree/shared/mem.h         \
        ../aul_tree/io/io.h              \
        ../aul_tree/io/io_shared.h

EXTRA_CFLAGS := -w -Wimplicit -Wstrict-prototypes -Wunused -O6 #-fomit-frame-pointer -pipe
EXTRA_CFLAGS += -march=i486
EXTRA_CFLAGS += -D$(DRV_NAME) $(DEBUG_ENABLE) $(SIMULATE_IO)

# if KERNELRELEASE is defined, we've been invoked from the
# kernel build system and can use its language.
ifneq (${KERNELRELEASE},)
  obj-m := mod_$(NAME).o
  mod_$(NAME)-objs := $(OBJS)
# Otherwise we were called directly from the command line.
# Invoke the kernel build system.
else
  KERNEL_SOURCE := /lib/modules/$(shell uname -r)/build
#using the eclipse generated "Debug" folder with all make files.
#  AUL_SOURCE := ../aul_tree/_Out_Auldrv_Kernel_Module
  PWD := $(shell pwd)
defaul:  ${HDR}
	echo "Drv_Test make - Compilation begin"
	${MAKE} -C ${KERNEL_SOURCE} M=${PWD} -I../aul_tree  modules
#	${MAKE} -C ${AUL_SOURCE} M=${PWD} all
	echo "Drv_Test make - default build complete."

clean:
	rm ${OBJS}
	${MAKE} -C ${KERNEL_SOURCE} SUBDIRS=${PWD} clean
#	${MAKE} -C ${AUL_SOURCE}  SUBDIRS=${PWD} clean
endif
