# Where appropriate, these must be specified in the includer:
# 
# 	sources = ( [ <source-file> ... ] | default)
# 		List of sources which should be compiled and added to the lib file. May
# 		be empty, in which case no lib will be produced. Default is all .cc and
# 		.asm files in the current directory.
#
# 	library = [ <name> | default ]
# 		Library name for the current directory. Default is name of the current
# 		directory. Can also be unspecified (eg, if current directory only has
# 		children. Governs two things:
# 			1. The name of the include directory into which exports are placed
# 			2. The name of the .lib file which is created for the sources
#
# 	children = ( [ <child-dir> ... ] | default )
# 	 Directories on which to perform recursive make. Default is all
# 	 subdirectories.

# Make sure .setup has been run (adds locations as environment variables).
ifeq "$(O)" ""
	$(error must run .setup)
endif

# Deal with arguments to make.
ifeq "$(MAKETYPE)" ""
 MAKETYPE = debug
endif
ifeq "$(ARCH)" ""
 ARCH = x86
endif

# Useful variables.
DIRNAME = $(notdir $(shell pwd))

# Tools.
CPP				= g++
AS				= nasm
LD				= ld
MV				= mv -f
RM				= rm -f
SED				= sed
CP				= cp
OBJCOPY 	= objcopy
MKDIR			= mkdir -p

# Build flags and stuff based on arguments.
CPPFLAGS	= -g -fno-exceptions -nostdlib -Wall -Werror -m32 \
			-fno-builtin -nostartfiles -nodefaultlibs -fno-rtti\
			-fno-stack-protector -I"$(OSK)" -include "stddefs/stdinc.h" -D$(ARCH)
LDFLAGS		= -m elf_i386 
ASMFLAGS	= -w+error -g

ifeq "$(MAKETYPE)" "debug"
 CPPFLAGS += -DDEBUG -DKDEBUG
endif

# Set defaults.
ifeq "$(sources)" "default"
 sources = $(wildcard *.cc) $(wildcard *.asm)
endif
ifeq "$(children)" "default"
 children = $(shell find * -maxdepth 0 -type d)
endif
ifeq "$(library)" "default"
 library = $(DIRNAME)
endif

# Sanity check.
ifneq "$(sources)" ""
 ifeq "$(library)" ""
  $(error library not specified)
 endif
endif

cc_sources	= $(filter %.cc,$(sources))
asm_sources	= $(filter %.asm,$(sources))
cc_objects	= $(subst .cc,.o,$(cc_sources))
asm_objects	= $(subst .asm,.asmobj,$(asm_sources))
objects			= $(cc_objects) $(asm_objects)
cc_depends	= $(subst .cc,.d,$(cc_sources))
libname			:= $(library)
library			= $(addsuffix .lib,$(addprefix $(OO)/,$(libname)))

.PHONY: build
build: $(children) $(library)

.PHONY: $(children)
$(children) :
	$(MAKE) --directory="$@" $(MAKECMDGOALS)

.PHONY: depends
depends: $(children) $(cc_depends)

.PHONY: clean
clean: $(children)
	$(RM) $(cc_objects) $(asm_objects) $(extra_clean)

.PHONY: clobber
clobber: $(children) clean
	$(RM) $(cc_depends) $(extra_clobber)

include $(addprefix $(OSO)/,addchildgoal.mk)

ifneq "$(MAKECMDGOALS)" "clean"
 ifneq "$(MAKECMDGOALS)" "depends"
  ifneq "$(MAKECMDGOALS)" "clobber"
	 include $(cc_dependencies)
  endif
 endif
endif

$(library) : $(objects)
	$(LD) -i $(LDFLAGS) -o $@ $(objects)

$(addprefix $(OOI)/,$(addprefix $(libname)/,%.h)) : %.h
	$(MKDIR) $(addprefix $(OOI)/,$(libname))
	$(CP) $< $@

%.d : %.cc
	$(CPP) -ec -M -MQ "$(subst .d,.o,$@) $@" $(CPPFLAGS) $< > $@

%.o : %.cc
	$(CPP) $(CPPFLAGS) -c -o $@ $<

%.asmobj : %.asm
	$(AS) $(ASMFLAGS) -f elf -o "$@" $<
