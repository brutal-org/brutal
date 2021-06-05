#include <library/base.h>
#include <library/io.h>

#include "arch.h"
#include "arch/asm.h"
#include "arch/com.h"
#include "arch/gdt.h"
#include "arch/idt.h"
#include "arch/stivale2.h"

void _start(struct stivale2_struct *info)
{
    UNUSED(info);

    com_initialize(COM1);
    gdt_initialize();
    idt_initialize();

    print(arch_log(), "Hello, {} {x}!\n", "pomme", 10);

    for (;;)
    {
        cli();
        hlt();
    }
}