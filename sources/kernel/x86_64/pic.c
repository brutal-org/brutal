#include "kernel/x86_64/pic.h"
#include "kernel/x86_64/asm.h"

void pic_disable(void)
{
    asm_out8(PIC2_DATA, 0xff);
    asm_out8(PIC1_DATA, 0xff);
}
