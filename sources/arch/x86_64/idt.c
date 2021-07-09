#include <brutal/log.h>
#include "arch/arch.h"
#include "arch/x86_64/idt.h"
#include "arch/x86_64/gdt.h"

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

        .code_segment = GDT_KERNEL_CODE*8,
        ._zero = 0,
    };
}

void idt_initialize(void)
{
    for (int i = 0; i < 48; i++)
    {
        idt.entries[i] = idt_entry(__interrupt_vector[i], 0, IDT_GATE);
    }

    idt.entries[100] = idt_entry(__interrupt_vector[48], 0, IDT_GATE | IDT_USER);
    idt.entries[101] = idt_entry(__interrupt_vector[49], 0, IDT_GATE | IDT_USER);

    idt_update(&idt_descriptor);
}
