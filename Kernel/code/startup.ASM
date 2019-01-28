format ELF

public _start
extrn __Z11kernel_mainPciii

section ".text" executable

_start:
        push       edx
        push       ecx
        push       ebx
        push       eax

        lgdt       [gdtr]
        call       __Z11kernel_mainPciii
 .bf:
        ;cli
        ;hlt
        jmp       .bf

section ".data" writable

gdt:
        dq 0                 
        dq 0x00CF9A000000FFFF
        dq 0x00CF92000000FFFF
gdtr:
        dw $ - gdt
        dd gdt