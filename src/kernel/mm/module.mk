# You shouldn't have to do anything special when copying this around, assuming
# the destination directory only has cc and nasm assembly files with the asm
# suffix.
#
# Any files specified in extra_clean will be deleted during a clean.

local_dir					:= $(dir $(lastword $(MAKEFILE_LIST)))
local_src_cc 			:= $(wildcard $(addprefix $(local_dir),*.cc))
local_src_asm			:= $(wildcard $(addprefix $(local_dir),*.asm))


sources    				+= $(local_src_cc)
sources						+= $(local_src_asm)
extra_clean 			+= 
