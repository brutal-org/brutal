#include <library/base/macros.h>
#include <library/log.h>

#include "arch/arch.h"
#include "kernel/kernel.h"

void kernel_splash(void)
{
    log("");
    log("   _____ _____ _____ _____ _____ __      ");
    log("  | __  | __  |  |  |_   _|  _  |  |     ");
    log("  | __ -|    -|  |  | | | |     |  |__   ");
    log("  |_____|__|__|_____| |_| |__|__|_____|  ");
    log("");
    log("-----------------------------------------");
}

void kernel_entry(struct handover *handover)
{
    UNUSED(handover);

    kernel_splash();
}

void cpu_entry(int id)
{
    log("loaded CPU {}!", id);

    while (true)
    {
        asm("hlt");
    }
}