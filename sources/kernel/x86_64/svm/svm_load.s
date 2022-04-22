

%macro SEGMENT_LOAD 2

    mov word [rdi + %0], %1 ; selector

    lsl eax, word [rdi + %0] ; limit
    mov dword [rdi + %0 + 4], eax
    mov word [rdi + %0 + 2], 0b10010010 ; generic RW + segment + present
%endmacro


global svm_load_asm
svm_load_asm: 

    SEGMENT_LOAD 0x0, es
    SEGMENT_LOAD 0x10, cs
    SEGMENT_LOAD 0x20, ss
    SEGMENT_LOAD 0x30, ds
    SEGMENT_LOAD 0x40, fs
    SEGMENT_LOAD 0x50, gs


    mov rax, cr4
    mov qword [rdi + 0x148], rax

    mov rax, cr3
    mov qword [rdi + 0x150], rax

    mov rax, cr0
    mov qword [rdi + 0x158], rax

    mov rax, dr7
    mov qword [rdi + 0x160], rax

    mov rax, dr6
    mov qword [rdi + 0x168], rax

    mov rax, 0
    mov qword [rdi + 0x240], rax


    ret

global svm_start
svm_start:
    mov rax, 0xffffffffffff
    jmp svm_start
    ret

save_addr:
    dq 0

global svm_run
svm_run:
    push rax

    mov rax, rdi
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    mov [rax + 0x16], rsp
    mov rsp, [rax + 0x8]


    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rbx
    push rax
    mov rax, [rax]
    vmload
    vmrun
    vmsave

    pop rax

    push r15
    push r14
    push r13
    push r12
    push r11
    push r10
    push r9
    push r8
    push rbp
    push rdi
    push rsi
    push rdx
    push rcx
    push rbx
    push rbx

    mov rsp, [rax + 0x16]

    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    ret