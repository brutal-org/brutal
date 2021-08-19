#include "kernel/x86_64/pic.h"
#include "kernel/x86_64/asm.h"

void pic_initialize(void)
{
    /* Cascade initialization */
    asm_out8(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    asm_out8(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    /* Remap */
    asm_out8(PIC1_DATA, PIC1_OFFSET);
    asm_out8(PIC2_DATA, PIC2_OFFSET);

    /* Cascade identity with slave PIC at IRQ2 */
    asm_out8(PIC1_DATA, 0x04);
    asm_out8(PIC2_DATA, 0x02);

    /* Request 8086 mode on each PIC */
    asm_out8(PIC1_DATA, 0x01);
    asm_out8(PIC2_DATA, 0x01);

    asm_out8(PIC1_DATA, 0x00);
    asm_out8(PIC2_DATA, 0x00);

    pic_disable();
}

void pic_eoi(int int_no)
{
    if (int_no >= 40)
    {
        asm_out8(PIC2, 0x20);
    }

    asm_out8(PIC1, 0x20);
}

void pic_disable(void)
{
    asm_out8(PIC2_DATA, 0xff);
    asm_out8(PIC1_DATA, 0xff);
}
