#include "kernel/riscv64/interrupts.h"
#include <brutal/debug.h>
#include "kernel/riscv64/asm.h"

extern void interrupt_common(void);

static char *_exception_messages[] = {
    "Instruction address misaligned",
    "Instruction access fault",
    "Illegal instruction",
    "Breakpoint",
    "Load address misaligned",
    "Load access fault",
    "Store/AMO address misaligned",
    "Store/AMO access fault",
    "Environment call from U-mode",
    "Environment call from S-mode",
    "Reserved",
    "Environment call from M-mode",
    "Instruction page fault",
    "Load page fault",
    "Reserved",
    "Store/AMO page fault",
};

static char *_motivation_messages[] = {
    "We'll patch this throught micro-transaction",
    "It just works!",
    "This is a feature",
    "TODO: fix this",
    "[ insert a joke here ]",
    "[ insert a devse private joke here ]",
    "<< Je connais cette théorie >> d0p1",
    "Welcome to paging hell !",
    "There is a bug in the matrix",
    "Calm down!",
    "RIP",
    "One day brutal will be bug free :tm:",
    "The error screen is my worst nightmare",
    "It will be easy they said...",
    "TIP: when pc = 0 there is a problem",
    "Let me interject for a moment what you call errors are in fact  brutal/errors",
    "Error don't exists",
    "What if everything was a dream ?",
    "If you do `make clean` the error will be fixed :tm:",
};

static void dump_register(RiscvRegs const *regs)
{
    log_unlock("cpu registers:");
    log_unlock("pc : {#016p}", read_csr(CSR_SEPC));
    log_unlock("stval: {#016p}", read_csr(CSR_STVAL));
    log_unlock("ra : {#016p} | sp : {#016p} | gp : {#016p} | tp : {#016p}", regs->ra, regs->sp, regs->gp, regs->tp);

    log_unlock("");

    log_unlock(" -- t0-t6 -- ");

    log_unlock("t0 : {#016p} | t1 : {#016p} | t2 : {#016p} | t3 : {#016p}", regs->t[0], regs->t[1], regs->t[2], regs->t2[0]);
    log_unlock("t4 : {#016p} | t5 : {#016p} | t6 : {#016p} ", regs->t2[1], regs->t2[2], regs->t2[3]);

    log_unlock("");
    log_unlock(" -- s0-s11 -- ");
    log_unlock("s0 : {#016p} | s1 : {#016p} | s2 : {#016p} | s3 : {#016p}", regs->s[0], regs->s[1], regs->s2[0], regs->s2[1]);
    log_unlock("s4 : {#016p} | s5 : {#016p} | s6 : {#016p} | s7 : {#016p}", regs->s2[2], regs->s2[3], regs->s2[4], regs->s2[5]);
    log_unlock("s8 : {#016p} | s9 : {#016p} | s10: {#016p} | s11: {#016p}", regs->s2[6], regs->s2[7], regs->s2[8], regs->s2[9]);

    log_unlock("");
    log_unlock(" -- a0-a7 -- ");
    log_unlock("a0 : {#016p} | a1 : {#016p} | a2 : {#016p} | a3 : {#016p}", regs->a[0], regs->a[1], regs->a[2], regs->a[3]);
    log_unlock("a4 : {#016p} | a5 : {#016p} | a6 : {#016p} | a7 : {#016p}", regs->a[4], regs->a[5], regs->a[6], regs->a[7]);
}

void interrupt_error_handler(RiscvRegs const *regs, int int_no)
{

    log_unlock("");
    log_unlock("------------------------------------------------------------");
    log_unlock("");
    log_unlock("KERNEL PANIC ON CPU N°{}", 0);
    log_unlock("");
    log_unlock("{}({}) !", _exception_messages[int_no], int_no);
    log_unlock("");

    dump_register(regs);

    log_unlock("");
    log_unlock(" - \"{}\"", _motivation_messages[regs->sp % (sizeof(_motivation_messages) / sizeof(_motivation_messages[0]))]);

    log_unlock("");
    log_unlock("------------------------------------------------------------");
}

void interrupt_handler(RiscvRegs *regs)
{
    uint64_t cause = read_csr(CSR_SCAUSE);
    int interrupt_id = cause & SCAUSE_CODE_MASK;

    if (cause & SCAUSE_INTERRUPT_MASK)
    {
        log_unlock("received interrupt: {}", interrupt_id);
    }
    else
    {
        log_unlock("received exception: {}", interrupt_id);
        interrupt_error_handler(regs, interrupt_id);

        while (true)
        {
        };
    }
}

void init_interrupts(void)
{
    log$("loading interrupt handler : {x}", (uint64_t)interrupt_common);
    write_csr(CSR_STVEC, (uint64_t)interrupt_common);
}
