.section .bss
.align 16

.globl __stack_bottom
.globl __stack_top

__stack_bottom:
.space 16384, 0
__stack_top:

.section .init
.extern arch_entry_main
.globl _kstart

_kstart:
    .cfi_startproc

    csrw satp, zero

    la sp, __stack_top

.option push
.option norelax
	la gp, __global_pointer$
.option pop

    tail arch_entry_main 
 
	.cfi_endproc

