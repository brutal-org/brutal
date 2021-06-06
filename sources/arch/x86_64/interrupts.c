#include <library/log.h>

#include "arch/x86_64/asm.h"
#include "arch/x86_64/interrupts.h"

static char *_exception_messages[32] = {
    "division-by-zero",
    "debug",
    "non-maskable-interrupt",
    "breakpoint",
    "detected-overflow",
    "out-of-bounds",
    "invalid-opcode",
    "no-coprocessor",
    "double-fault",
    "coprocessor-segment-overrun",
    "bad-tss",
    "segment-not-present",
    "stack-fault",
    "general-protection-fault",
    "page-fault",
    "unknown-interrupt",
    "coprocessor-fault",
    "alignment-check",
    "machine-check",
    "reserved",
    "reserved",
    "reserved",
    "reserved",
    "reserved",
    "reserved",
    "reserved",
    "reserved",
    "reserved",
    "reserved",
    "reserved",
    "reserved",
    "reserved",
};

void dump_register(struct interrupt_stackframe const *stackframe)
{
    uint64_t CR2 = 0;

    asm volatile("mov %%cr2, %0"
                 : "=r"(CR2));
    uint64_t CR3 = 0;

    asm volatile("mov %%cr3, %0"
                 : "=r"(CR3));

    log("rax: 0x{pa} | rbx: 0x{pa} | rcx: 0x{pa}", stackframe->rax, stackframe->rbx, stackframe->rcx);
    log("rdx: 0x{pa} | rsi: 0x{pa} | rdi: 0x{pa}", stackframe->rdx, stackframe->rsi, stackframe->rdi);
    log("r8 : 0x{pa} | r9 : 0x{pa} | r10: 0x{pa}", stackframe->r8, stackframe->r9, stackframe->r10);
    log("r11: 0x{pa} | r12: 0x{pa} | r13: 0x{pa}", stackframe->r11, stackframe->r12, stackframe->r13);
    log("r14: 0x{pa} | r15: 0x{pa}", stackframe->r14, stackframe->r15);
    log("rip: 0x{pa} | rsp: 0x{pa} | rbp: 0x{pa}", stackframe->rip, stackframe->rsp, stackframe->rbp);
    log("cs : 0x{pa} | ss : 0x{pa} | rflags: 0x{pa}", stackframe->cs, stackframe->ss, stackframe->rflags);
    log("cr2: 0x{pa} | cr3: 0x{pa} ", CR2, CR3);
}

void *interrupt_handler(struct interrupt_stackframe *stackframe)
{
    if (stackframe->int_no < 32)
    {
        log("Interrupt {}: {}: error: {} on {x} !", stackframe->int_no, _exception_messages[stackframe->int_no], stackframe->error_code, stackframe->rip);

        dump_register(stackframe);

        while (true)
        {
            cli();
            hlt();
        }
    }

    return stackframe;
}