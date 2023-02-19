#include "kernel/x86_64/idt.h"
#include <brutal-debug>
#include "kernel/arch.h"
#include "kernel/x86_64/apic.h"
#include "kernel/x86_64/gdt.h"

extern uintptr_t __interrupt_vector[256];

static Idt _idt = {};

static IdtDesc _idt_desc = {
    .size = sizeof(Idt) - 1,
    .offset = (uintptr_t)&_idt,
};

IdtEntry idt_entry(uintptr_t handler, uint8_t ist, uint8_t idt_flags)
{
    return (IdtEntry){
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
    for (int i = 0; i < 256; i++)
    {
        _idt.entries[i] = idt_entry(__interrupt_vector[i], 0, IDT_GATE);
    }

    // Specials cases for IPIs dans the timer IRQ.
    _idt.entries[32] = idt_entry(__interrupt_vector[32], 0, IDT_GATE);
    _idt.entries[IPI_RESCHED] = idt_entry(__interrupt_vector[IPI_RESCHED], 0, IDT_GATE);
    _idt.entries[IPI_STOP] = idt_entry(__interrupt_vector[IPI_STOP], 1, IDT_GATE);

    idt_update(&_idt_desc);
}
