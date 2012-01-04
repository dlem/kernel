# Clearly, this makefile isn't really architecture-independent, but the source
# code is meant to be laid out in a way conductive to future expansion - hence
# the ARCH variable.

ARCH						=x86
DEBUG						=1


CPP							= g++
CPPFLAGS				= -g -fno-exceptions -nostdlib -Wall -Werror \
										-fno-builtin -nostartfiles -nodefaultlibs -fno-rtti\
										-fno-stack-protector -DKDEBUG=${DEBUG}
										
ASMFLAGS				= -w+error -g
PROJECT_DIR			=./
BUILD_DIR				=$(addprefix $(PROJECT_DIR),build/)
BIN_DIR					=$(addprefix $(PROJECT_DIR),bin/)
DEBUG_DIR				=$(BUILD_DIR)
GRUB_DIR				=$(addprefix $(PROJECT_DIR),grub/)
ARCH_DIR				=$(addprefix $(addprefix src/kernel/arch/,$(ARCH)),/)

$(info Building OS with ARCH=$(ARCH), DEBUG=$(DEBUG))

# include dirs:
#		1. Kernel include dir. The arch dir here contains "interface" header files that should be 
#			implemented in the architecture directory. These require "arch/" when including.
#		2. Resources include dir.
#		3. Arch-specific kernel includes, under include/arch/$(ARCH)/. These don't require the "arch/" when including.
#		4. Arch include dir. The kernel proper SHOULD NOT include from here, but I don't know how to
# 		partition it properly without making my life hard.
include_dirs 			= src/kernel/include src/res/include $(addprefix $(ARCH_DIR),include)
CPPFLAGS					+= $(addprefix -I,$(include_dirs))
vpath %.h $(arch_include_dirs)							# add includes to prereq search path

MV							:= mv -f
RM							:= rm -f
SED							:= sed

programs=$(addprefix $(BIN_DIR),os.iso)
debug=$(addprefix $(BUILD_DIR),os.debug)
sources					:=
extra_clean			:=

all:

include src/kernel/kstdlib/module.mk
include src/kernel/main/module.mk
include src/kernel/util/module.mk
include src/kernel/mm/module.mk

# Architecture specific includes
include $(addprefix $(ARCH_DIR),module.mk)

cc_sources			:= $(filter %.cc,$(sources))
asm_sources			:= $(filter %.asm,$(sources))
cc_objects		   = $(subst .cc,.o,$(cc_sources))
asm_objects		   = $(subst .asm,.asmobj,$(asm_sources))
cc_dependencies	 = $(subst .cc,.d,$(cc_sources))

all: $(programs) $(debug)

.PHONY: cleanbuild

cleanbuild:
	make clean
	make all


.PHONY: clean
clean:
	$(RM) $(cc_objects) $(asm_objects) $(programs) $(debug) $(cc_dependencies) $(extra_clean)

.PHONY: depends
depends: $(cc_dependencies)


ifneq "$(MAKECMDGOALS)" "clean"
 ifneq "$(MAKECMDGOALS)" "depends"
  include $(cc_dependencies)
 endif
endif

%.d : %.cc
	$(CPP) -ec -M -MQ "$(subst .d,.o,$@) $@" $(CPPFLAGS) $< > $@

%.o : %.cc
	$(CPP) $(CPPFLAGS) -c -o $@ $<

%.asmobj : %.asm
	nasm $(ASMFLAGS) -f elf -o "$@" $<

$(addprefix $(BUILD_DIR),os.elf) : $(addprefix $(BUILD_DIR),os_unstripped.elf)
	objcopy --strip-debug $(addprefix $(BUILD_DIR),os_unstripped.elf) $@
extra_clean += $(addprefix $(BUILD_DIR),os.elf)

$(addprefix $(BUILD_DIR),os_unstripped.elf) : $(cc_objects) $(asm_objects)
	ld -T src/build/linker_script.lds -o $@ $(cc_objects) $(asm_objects)
extra_clean += $(addprefix $(BUILD_DIR),os_unstripped.elf) 

$(addprefix $(BIN_DIR),os.iso) : $(addprefix $(BUILD_DIR),os.elf) $(addprefix $(GRUB_DIR),menu.lst) $(addprefix $(GRUB_DIR),stage2_eltorito)
	genisoimage -graft-points -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -boot-info-table -o "$@" \
		boot/os.elf=$(addprefix $(BUILD_DIR),os.elf)\
		boot/grub/menu.lst=$(addprefix $(GRUB_DIR),menu.lst)\
		boot/grub/stage2_eltorito=$(addprefix $(GRUB_DIR),stage2_eltorito)
extra_clean += $(addprefix $(BIN_DIR),os.iso)

$(addprefix $(BUILD_DIR),os.debug) : $(addprefix $(BUILD_DIR),os_unstripped.elf) $(addprefix $(BUILD_DIR),os.elf)
	objcopy --only-keep-debug $(addprefix $(BUILD_DIR),os_unstripped.elf) $(addprefix $(DEBUG_DIR),os.debug)
	objcopy --add-gnu-debuglink=$(addprefix $(DEBUG_DIR),os.debug) $(addprefix $(BUILD_DIR),os.elf)
