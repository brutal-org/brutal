#include <brutal-debug>
#include <brutal-sync>
#include <brutal-time>

#include "kernel/x86_64/apic.h"
#include "kernel/x86_64/asm.h"
#include "kernel/x86_64/cmos.h"
#include "kernel/x86_64/interrupts.h"
#include "kernel/x86_64/pic.h"
#include "kernel/x86_64/simd.h"
#include "kernel/x86_64/smp.h"
#include "kernel/x86_64/syscall.h"

#include "kernel/context.h"
#include "kernel/cpu.h"
#include "kernel/event.h"
#include "kernel/sched.h"

static char *_exception_messages[32] = {
    "DivisionByZero",
    "Debug",
    "NonMaskableInterrupt",
    "Breakpoint",
    "DetectedOverflow",
    "OutOfBounds",
    "InvalidOpcode",
    "NoCoprocessor",
    "DoubleFault",
    "CoprocessorSegmentOverrun",
    "BadTss",
    "SegmentNotPresent",
    "StackFault",
    "GeneralProtectionFault",
    "PageFault",
    "UnknownInterrupt",
    "CoprocessorFault",
    "AlignmentCheck",
    "MachineCheck",
    "SIMD Floating-Point Exception",
    "Virtualization Exception",
    "Control Protection Exception",
    "Reserved",
    "Hypervisor Injection Exception",
    "VMM Communication Exception",
    "Security Exception",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
};

struct _StackFrame
{
    struct _StackFrame *rbp;
    uint64_t rip;
};

static bool _has_panic = false;

static size_t dump_backtrace(uintptr_t rbp)
{
    struct _StackFrame *stackframe = (struct _StackFrame *)rbp;

    size_t i = 0;

    while (stackframe)
    {
        log_unlock("- {p}", stackframe->rip);
        stackframe = stackframe->rbp;
    }

    return i;
}

static void dump_register(Regs const *regs)
{
    log_unlock("RIP: {#016p} | RSP: {#016p}", regs->rip, regs->rsp);
    log_unlock("CR2: {#016p} | CR3: {#016p} ", asm_read_cr2(), asm_read_cr3());
    log_unlock("CS : {#02p} | SS : {#02p} | RFlags: {#p}", regs->cs, regs->ss, regs->rflags);

    log_unlock("");

    log_unlock("RAX: {#016p} | RBX: {#016p}", regs->rax, regs->rbx);
    log_unlock("RCX: {#016p} | RDX: {#016p}", regs->rcx, regs->rdx);
    log_unlock("RSI: {#016p} | RDI: {#016p}", regs->rsi, regs->rdi);
    log_unlock("RBP: {#016p} | R8 : {#016p}", regs->rbp, regs->r8);
    log_unlock("R9 : {#016p} | R10: {#016p}", regs->r9, regs->r10);
    log_unlock("R11: {#016p} | R12: {#016p}", regs->r11, regs->r12);
    log_unlock("R13: {#016p} | R14: {#016p}", regs->r13, regs->r14);
    log_unlock("R15: {#016p}", regs->r15);
}

static void interrupt_error_handler(Regs *regs)
{
    if (_has_panic)
    {
        log_unlock("[NESTED PANIC]");
        while (true)
        {
            asm_cli();
            asm_hlt();
        }
        return;
    }

    _has_panic = true;

    smp_stop_all();

    log_unlock("");

    log_unlock("------------------------------------------------------------");
    log_unlock("");

    log_unlock("KERNEL PANIC ON CPU N°{}", cpu_self_id());
    log_unlock("");

    log_unlock("{}({}) with error_code={}!", _exception_messages[regs->int_no], regs->int_no, regs->error_code);
    log_unlock("");

    if (task_self() != nullptr)
    {
        log_unlock("Running task is {}", task_self()->id);
        log_unlock("");
    }

    dump_register(regs);
    log_unlock("");

    dump_backtrace(regs->rbp);
    log_unlock("");

    log_unlock("------------------------------------------------------------");
    log_unlock("");

    while (true)
    {
        asm_cli();
        asm_hlt();
    }
}

uint64_t interrupt_handler(uint64_t rsp)
{
    Regs *regs = (Regs *)rsp;

    if (regs->int_no < 32)
    {
        interrupt_error_handler(regs);
    }

    cpu_begin_interrupt();

    if (regs->int_no >= 32 && regs->int_no <= 48)
    {
        event_trigger((BrEvent){.type = BR_EVENT_IRQ, .irq = regs->int_no - 32});
    }

    if (regs->int_no == 32)
    {
        context_save(task_self()->context, regs);
        sched_schedule();
        sched_switch();
        context_load(task_self()->context, regs);

        space_switch(task_self()->space);
    }
    else if (regs->int_no == IPI_RESCHED)
    {
        context_save(task_self()->context, regs);
        sched_switch();
        context_load(task_self()->context, regs);

        space_switch(task_self()->space);
    }
    else if (regs->int_no == IPI_STOP)
    {
        while (true)
        {
            asm_cli();
            asm_hlt();
        }
    }
    else if (regs->int_no == 0xf0)
    {
        log_unlock("Non maskable interrupt from APIC (Possible hardware error).");
    }

    cpu_end_interrupt();

    lapic_eoi();

    return rsp;
}
