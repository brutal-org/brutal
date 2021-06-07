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

    log("rax: {#016p} | rbx: {#016p} | rcx: {#016p}", stackframe->rax, stackframe->rbx, stackframe->rcx);
    log("rdx: {#016p} | rsi: {#016p} | rdi: {#016p}", stackframe->rdx, stackframe->rsi, stackframe->rdi);
    log("r8 : {#016p} | r9 : {#016p} | r10: {#016p}", stackframe->r8, stackframe->r9, stackframe->r10);
    log("r11: {#016p} | r12: {#016p} | r13: {#016p}", stackframe->r11, stackframe->r12, stackframe->r13);
    log("r14: {#016p} | r15: {#016p}", stackframe->r14, stackframe->r15);
    log("rip: {#016p} | rsp: {#016p} | rbp: {#016p}", stackframe->rip, stackframe->rsp, stackframe->rbp);
    log("cs : {#016p} | ss : {#016p} | rflags: {#016p}", stackframe->cs, stackframe->ss, stackframe->rflags);
    log("cr2: {#016p} | cr3: {#016p} ", CR2, CR3);
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