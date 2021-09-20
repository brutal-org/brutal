

.globl _kstart
_kstart:
.option push
.option norelax
	la gp, __global_pointer$
.option pop
    jal arch_entry_main 

    
