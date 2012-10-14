global outportb
global outportw
global outportdw
global inportb
global inportw
global inportdw

outportb:
	mov eax, [esp + 4]
	mov edx, [esp + 8]
	out dx, al
	ret

outportw:
	mov eax, [esp + 4]
	mov edx, [esp + 8]
	out dx, ax
	ret

outportdw:
	mov eax, [esp + 4]
	mov edx, [esp + 8]
	out dx, eax
	ret

inportb:
	mov edx, [esp + 4]
	in al, dx
	ret

inportw:
	mov edx, [esp + 4]
	in ax, dx
	ret

inportdw:
	mov edx, [esp + 4]
	in eax, dx
	ret

	global asm_get_eflags, CLI, STI

CLI:
  cli
  ret

STI:
  sti
  ret

asm_get_eflags:
  pushfd
  pop eax
  ret
