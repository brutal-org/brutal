

%macro SEGMENT_LOAD 2

    mov word [rdi + %0], %1 ; selector

    lsl eax, word [rdi + %0] ; limit
    mov dword [rdi + %0 + 4], eax

%endmacro


global svm_load_asm
svm_load_asm: 

    SEGMENT_LOAD 0x0, es
    SEGMENT_LOAD 0x10, cs
    SEGMENT_LOAD 0x20, ss
    SEGMENT_LOAD 0x30, ds
    SEGMENT_LOAD 0x40, fs
    SEGMENT_LOAD 0x50, gs

    sgdt [rdi + 0x60]
    sldt [rdi + 0x70]
    sidt [rdi + 0x80]

    mov ecx, 0x0c0000080
    rdmsr
    ; efer
    mov [rdi + 0xd0], eax
    mov [rdi + 0xd4], edx

    mov rax, cr4
    mov [rdi + 0x148],rax

    mov rax, cr3
    mov [rdi + 0x150],rax

    mov rax, cr0
    mov [rdi + 0x158],rax

    mov rax, dr7
    mov [rdi + 0x160],rax

    mov rax, dr6
    mov [rdi + 0x168],rax


    ret

