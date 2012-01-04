# This file should be put in new arch subdirectories. Feel free to add includes.

arch_dir					:= $(dir $(lastword $(MAKEFILE_LIST)))

include $(addprefix $(arch_dir),hardware/module.mk)
include $(addprefix $(arch_dir),main/module.mk)
include $(addprefix $(arch_dir),sync/module.mk)