#include <base-macros.h>

#include "arch/com.h"
#include "arch/stivale2.h"

void _start(struct stivale2_struct *stivale2_struct)
{
    UNUSED(stivale2_struct);

    com_initialize(COM1);
    com_write(COM1, "Hello, world!\n", 15);

    for (;;)
    {
        asm("hlt");
    }
}