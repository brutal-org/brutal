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

    csrw satp, zero # turn off paging

    la sp, __stack_top 

.option push
.option norelax
	la gp, __global_pointer$
.option pop


    # zero bss

    la t0, metal_segment_bss_target_start
    la t1, metal_segment_bss_target_end
    beq t0, t1, 2f
1:  sw zero, 0(t0)
    addi t0, t0, 8
    bne t0, t1, 1b
2:
    call arch_entry_main 
    
hang:
    j hang
	.cfi_endproc

