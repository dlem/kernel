global gdt_spec
global SYS_DATA_SEL
global SYS_CODE_SEL


; 48-bit GDT spec to be loaded with lgdt
gdt_spec:
        dw gdt_end - gdt - 1 ; 16-bit size of gdt
        dd gdt ; location in memory of gdt

; this global descriptor table has three 8-byte entries - one null descriptor
; for memory protection features and two flat (0 -> 0xFFFFFFFF) segments
; for the OS; one code segment and one data segment
gdt:
;;;;;;; ; null descriptor
        times 8 db 0

;;;;;;; ; code segment descriptor
        SYS_CODE_SEL equ $ - gdt
        ; limit_low: 0xFFFF
        dw 0xFFFF
        ; base_low: 0
        dw 0
        ; base_middle: 0
        db 0
        ; Present=1, ring=0, type=non-system, type=code (can't figure out rest)
        db 0x9A ; access byte
        ; 4KB, 32-bit, zero, system access=0, limit_high=0xF
        db 0xCF ; granularity byte
        db 0 ; base high

;;;;;;; ; data segment descriptor
        SYS_DATA_SEL equ $ - gdt
        dw 0xFFFF ; limit_low
        dw 0 ; base_low
        db 0 ; base_middle
        db 0x92 ; access byte - same as for code segment but data segment
        db 0xCF ; gran
        db 0 ; base high

gdt_end:
