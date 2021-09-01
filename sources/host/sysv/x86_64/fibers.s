

global fibers_switch
fibers_switch:
    mov r8, [rsp]
    mov [rdi] , r8

    lea r8, [rsp+8]
    mov [rdi+8], r8


    mov [rdi+16], rbx
    mov [rdi+24], rbp
    mov [rdi+32], r12
    mov [rdi+40], r13
    mov [rdi+48], r14
    mov [rdi+56], r15
    stmxcsr [rdi+64]
    fnstcw [rdi+68]

    mov rsp, [rsi+8]

    mov rbx, [rsi+16]
    mov rbp, [rsi+24]
    mov r12, [rsi+32]
    mov r13, [rsi+40]
    mov r14, [rsi+48]
    mov r15, [rsi+56]

    ldmxcsr [rsi+64]
    fldcw [rsi+68]
    mov r8, [rsi]
    push r8

    ret
