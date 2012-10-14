OS_ISO	:= $(addprefix $(OB)/,os.iso)
OS_ELF	:= $(addprefix $(OB)/,os.elf)
OS_DBG	:= $(addprefix $(OB)/,os.debug)
OS_UNS	:= $(addprefix $(OB)/,os_unstripped.elf)
ELTORITO	:= $(addprefix $(OG)/,stage2_eltorito)
MENULST		:= $(addprefix $(OG)/,menu.lst)

children = kernel
extra_clobber += $(OS_ISO) $(OS_ELF) $(OS_DBG) $(OS_UNS)
include $(addprefix $(OSO),/include.mk)

build : $(OS_ISO) $(OS_DBG)

$(OS_ELF) : $(OS_UNS)
	$(OBJCOPY) --strip-debug $< $@

.PHONY: $(OS_UNS)
$(OS_UNS) : $(build)
	$(LD) -T $(addprefix $(OSO)/,linker_script.lds) $(LDFLAGS) -o $@ $(wildcard $(OO)/*.lib)

$(OS_ISO) : $(OS_ELF) $(MENULST) $(ELTORITO)
	genisoimage -graft-points -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -boot-info-table -o "$@" \
		boot/os.elf=$(OS_ELF)\
		boot/grub/menu.lst=$(MENULST)\
		boot/grub/stage2_eltorito=$(ELTORITO)

$(OS_DBG) : $(OS_UNS) $(OS_ELF)
	objcopy --only-keep-debug $(OS_UNS) $@
	objcopy --add-gnu-debuglink=$@ $(OS_ELF)
