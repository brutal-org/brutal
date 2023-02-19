TRAMPOLINE_BASE equ 0x1000

SMP_INIT_PAGE_TABLE equ 0x500
SMP_INIT_ENTRY equ 0x510
SMP_INIT_STACK equ 0x520
SMP_INIT_GDT equ 0x530
SMP_INIT_IDT equ 0x540

;; --- 16 BIT --------------------------------------------------------------- ;;

[bits 16]
global trampoline_start
trampoline_start:
    cli
    mov ax, 0x0
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    o32 lgdt [pm_gdtr - trampoline_start + TRAMPOLINE_BASE]

    ; enable protected mode
    mov eax, cr0
    or al, 0x1
    mov cr0, eax

    jmp 0x8:(trampoline32 - trampoline_start + TRAMPOLINE_BASE)

;; --- 32 BITS -------------------------------------------------------------- ;;

[bits 32]
section .text
trampoline32:
    mov bx, 0x10
    mov ds, bx
    mov es, bx
    mov ss, bx

    ; Load page table
    mov eax, dword [SMP_INIT_PAGE_TABLE]
    mov cr3, eax

    ; Set the PAE-bit, which is the 6th bit (bit 5).
    mov eax, cr4
    or eax, 1 << 5
    or eax, 1 << 7
    mov cr4, eax

    ; Efet register
    mov ecx, 0xc0000080
    rdmsr

    ; Long mode enable
    or eax,1 << 8
    wrmsr

    ; Enable paging
    mov eax, cr0
    or eax, 1 << 31 
    mov cr0, eax


    lgdt [lm_gdtr - trampoline_start + TRAMPOLINE_BASE]
    jmp 8:(trampoline64 - trampoline_start + TRAMPOLINE_BASE)

;; ---- 64 BITS ------------------------------------------------------------- ;;

[bits 64]
trampoline64:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov ax, 0x0
    mov fs, ax
    mov gs, ax

    lgdt [SMP_INIT_GDT]
    lidt [SMP_INIT_IDT]

    ; Setup the stack
    mov rsp, [SMP_INIT_STACK]
    mov rbp, 0x0

    ; reset RFLAGS
    push 0x0
    popf

    mov rax, [SMP_INIT_ENTRY]
    call rax

;; ---- Protected Mode GDT/IDT ---------------------------------------------- ;;

align 16
  pm_gdtr:
    dw pm_gdt_end - pm_gdt_start - 1
    dd pm_gdt_start - trampoline_start + TRAMPOLINE_BASE

align 16
  pm_gdt_start:
    dq 0                  ; null selector
    dq 0x00CF9A000000FFFF ; cs selector
    dq 0x00CF92000000FFFF ; ds selector
  pm_gdt_end:

align 16
  pm_idtr:
    dw 0
    dd 0
    dd 0
    align 16

;; ---- Long Mode GDT/IDT --------------------------------------------------- ;;

align 16
  lm_gdtr:
    dw lm_gdt_end - lm_gdt_start - 1
    dq lm_gdt_start - trampoline_start + TRAMPOLINE_BASE

align 16
  lm_gdt_start:
    dq 0                  ; null selector
    dq 0x00AF98000000FFFF ; 64bit cs selector
    dq 0x00CF92000000FFFF ; 64bit ds selector
  lm_gdt_end:

global trampoline_end
trampoline_end:
