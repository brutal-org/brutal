.section .text
.align 16

.globl __stack_bottom
.globl __stack_top

__stack_bottom:
.space 16384, 0
__stack_top:

.section .init
.extern arch_entry_main
.extern _kernel_end
.extern _kernel_start
.extern kernel_end
.extern kernel_start
.globl _kstart

.extern __global_pointer$

_kstart:
    .cfi_startproc
    csrw satp, zero # turn off paging

    # load stack relative to pc
3:
    auipc sp, %pcrel_hi(__stack_top)
    addi sp, sp, %pcrel_lo(3b)

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

    # load segments relative to pc
4:
    auipc a3, %pcrel_hi(_kernel_end)
    addi a3, a3, %pcrel_lo(4b)
5:
    auipc a2, %pcrel_hi(_kernel_start)
    addi a2, a2, %pcrel_lo(5b)


    j arch_entry_main
hang:
    j hang
	.cfi_endproc

