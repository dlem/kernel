#!/bin/bash
gnome-terminal -x ./bochs-gdb.sh &
gnome-terminal -x gdb ../../build/os_unstripped.elf
