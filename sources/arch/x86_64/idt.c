#include <library/log.h>

#include "arch/arch.h"
#include "arch/x86_64/idt.h"

extern uintptr_t __interrupt_vector[128];

static struct idt idt = {};

static struct idt_descriptor idt_descriptor = {
    .size = sizeof(struct idt) - 1,
    .offset = (uintptr_t)&idt,
};

struct idt_entry idt_entry(uintptr_t handler, uint8_t ist, uint8_t idt_flags)
{
    return (struct idt_entry){
        .attributes = idt_flags,
        .ist = ist,

        .offset_0_16 = (handler),
        .offset_16_32 = (handler >> 16),
        .offset_32_63 = (handler >> 32),

        .code_segment = 0x8,
        ._zero = 0,
    };
}

void idt_initialize(void)
{
    for (int i = 0; i < 48; i++)
    {
        idt.entries[i] = idt_entry(__interrupt_vector[i], 0, IDT_GATE);
    }

    // FIXME: Not used for the moment

    // syscall interrupt
    //idt[127] = idt_entry((void *)__interrupt_vector[48], 0, IDT_GATE | IDT_USER);

    // multi processor signal interrupt
    //idt[100] = idt_entry((void *)__interrupt_vector[49], 1, IDT_GATE | IDT_USER);

    idt_update(&idt_descriptor);
}

void *interrupt_handler(struct interrupt_stackframe *stackframe)
{
    log("Interrupt {} !", stackframe->int_no);
    return stackframe;
}