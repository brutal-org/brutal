#include <brutal/log.h>
#include <brutal/sync.h>
#include "arch/cpu.h"
#include "arch/x86_64/apic.h"
#include "arch/x86_64/asm.h"
#include "arch/x86_64/interrupts.h"
#include "arch/x86_64/pic.h"
#include "brutal/types.h"
#include "kernel/tasking.h"

struct lock error_lock;

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
    log_unlock("rax: {#016p} | rbx: {#016p} | rcx: {#016p}", stackframe->rax, stackframe->rbx, stackframe->rcx);
    log_unlock("rdx: {#016p} | rsi: {#016p} | rdi: {#016p}", stackframe->rdx, stackframe->rsi, stackframe->rdi);
    log_unlock("r8 : {#016p} | r9 : {#016p} | r10: {#016p}", stackframe->r8, stackframe->r9, stackframe->r10);
    log_unlock("r11: {#016p} | r12: {#016p} | r13: {#016p}", stackframe->r11, stackframe->r12, stackframe->r13);
    log_unlock("r14: {#016p} | r15: {#016p}", stackframe->r14, stackframe->r15);
    log_unlock("rip: {#016p} | rsp: {#016p} | rbp: {#016p}", stackframe->rip, stackframe->rsp, stackframe->rbp);
    log_unlock("cs : {#016p} | ss : {#016p} | rflags: {#016p}", stackframe->cs, stackframe->ss, stackframe->rflags);
    log_unlock("cr2: {#016p} | cr3: {#016p} ", asm_read_cr2(), asm_read_cr3());
}

void interrupt_error_stop_all_cpu()
{
    for (cpu_id_t i = 0; i < cpu_count(); i++)
    {
        if (i != cpu_self_id())
        {
            apic_send_ipit(i, IPIT_STOP);
        }
    }
}

void interrupt_error_handler(struct interrupt_stackframe *stackframe, uintptr_t rsp)
{
    lock_acquire(&error_lock);

    interrupt_error_stop_all_cpu();

    log_unlock("CPU {} PANIC RSP={p}", cpu_self_id(), rsp);
    log_unlock("Interrupt {}: {}: error: {} on {x} !", stackframe->int_no, _exception_messages[stackframe->int_no], stackframe->error_code, stackframe->rip);

    dump_register(stackframe);

    while (true)
    {
        asm_cli();
        asm_hlt();
    }
}

uint64_t interrupt_handler(uint64_t rsp)
{
    auto stackframe = (struct interrupt_stackframe *)rsp;

    if (stackframe->int_no < 32)
    {
        interrupt_error_handler(stackframe, rsp);
    }
    else if (stackframe->int_no == 32)
    {
        rsp = scheduler_schedule_and_switch(rsp);
    }
    else if (stackframe->int_no == IPIT_RESCHED)
    {
        rsp = scheduler_switch(rsp);
    }
    else if (stackframe->int_no == IPIT_STOP)
    {
        while (true)
        {
            asm_cli();
            asm_hlt();
        }
    }
    else if (stackframe->int_no == 0xf0)
    {
        log_unlock("non maskable interrupt from apic: possible hardware error");
    }

    apic_eoi();

    return rsp;
}
