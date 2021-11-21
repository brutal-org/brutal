

#include "kernel/riscv64/uart.h"
#include "kernel/riscv64/interrupts.h"
#include <brutal/debug.h>

void test()
{
    log$("hey hey");
}
void arch_entry_main(void)
{
    uart_init();
    log$("hello world");
    test();
    init_interrupts();
    log$("loaded interrupts");
    while (1)
    {
    }
}
