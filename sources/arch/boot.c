#include <library/base.h>
#include <library/io.h>

#include "arch.h"
#include "arch/com.h"
#include "arch/gdt.h"
#include "arch/stivale2.h"

void _start(struct stivale2_struct *stivale2_struct)
{
    UNUSED(stivale2_struct);

    com_initialize(COM1);
    gdt_initialize();

    print(arch_log(), "Hello, {}!\n", "pomme");

    for (;;)
    {
        asm("hlt");
    }
}