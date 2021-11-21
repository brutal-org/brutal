

#include "kernel/riscv64/uart.h"
#include <brutal/debug.h>
void arch_entry_main(void)
{
    uart_init();
    log$("hello world");
    while (1)
    {
    }
}
