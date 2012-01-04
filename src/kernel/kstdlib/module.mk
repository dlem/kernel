local_dir					:= $(dir $(lastword $(MAKEFILE_LIST)))
local_src_cc 			:= $(wildcard $(addprefix $(local_dir),*.cc))
local_src_asm			:= $(wildcard $(addprefix $(local_dir),*.asm))


sources    				+= $(local_src_cc)
sources						+= $(local_src_asm)
extra_clean 			+= 
