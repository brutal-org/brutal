section .bss
align 16

global __stack_bottom
global __stack_top

__stack_bottom:
resb 16384 ; 16 KiB
__stack_top:

section .text

global _kstart:function (_kstart.end - _kstart)
_kstart:
    cli
	cld

    mov rsp, __stack_top
	xor rbp, rbp

    extern arch_entry_main
    call arch_entry_main

.hang:
	cli
	hlt
	jmp .hang
.end:

extern arch_entry_main_stivale2

section .data

__stivale2_framebuffer_tag:
    dq 0x3ecc1bc43d0f7971
    dq 0
    dw 0
    dw 0
    dw 32

section .stivale2hdr
align 4
__stivale2_header:
    dq arch_entry_main_stivale2
    dq __stack_top
    dq 0
    dq __stivale2_framebuffer_tag

