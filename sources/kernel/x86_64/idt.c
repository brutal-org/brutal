#include <brutal/log.h>
#include "kernel/arch.h"
#include "kernel/x86_64/apic.h"
#include "kernel/x86_64/gdt.h"
#include "kernel/x86_64/idt.h"

extern uintptr_t __interrupt_vector[256];

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

        .code_segment = GDT_KERNEL_CODE * 8,
        ._zero = 0,
    };
}

void idt_initialize(void)
{
    for (int i = 0; i < 48; i++)
    {
        idt.entries[i] = idt_entry(__interrupt_vector[i], 0, IDT_GATE);
    }

    for (int i = 48; i < 256; i++)
    {
        idt.entries[i] = idt_entry(__interrupt_vector[i], 0, IDT_GATE);
    }

    // Specials cases for IPIs dans the timer IRQ.
    idt.entries[32] = idt_entry(__interrupt_vector[32], 1, IDT_GATE);
    idt.entries[IPI_RESCHED] = idt_entry(__interrupt_vector[IPI_RESCHED], 1, IDT_GATE);
    idt.entries[IPI_STOP] = idt_entry(__interrupt_vector[IPI_STOP], 0, IDT_GATE);

    idt_update(&idt_descriptor);
}
