#include <library/base.h>
#include <library/io.h>

#include "arch.h"
#include "arch/com.h"
#include "arch/gdt.h"
#include "arch/idt.h"
#include "arch/pic.h"
#include "arch/stivale2.h"

void arch_entry(struct handover *handover)
{
    UNUSED(handover);

    com_initialize(COM1);
    gdt_initialize();
    idt_initialize();
    pic_disable();
    pic_disable();

    print(arch_debug(), "Hello, {} {x}!\n", "pomme", 10);
}