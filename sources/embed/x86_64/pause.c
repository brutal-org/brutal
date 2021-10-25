#include <embed/arch.h>

void arch_pause(void)
{
    asm volatile("pause");
}
