

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

    mov ecx, 0x0c0000080
    rdmsr
    ; efer
    mov dword [rdi + 0xd0], eax
    mov dword [rdi + 0xd4], edx

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
    jmp svm_start
    ret