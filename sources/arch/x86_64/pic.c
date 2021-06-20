#include "arch/x86_64/pic.h"
#include "arch/x86_64/asm.h"

void pic_wait(void)
{
    asm volatile("jmp 1f\n\t"
                 "1:\n\t"
                 "    jmp 2f\n\t"
                 "2:");
}

void pic_initialize(void)
{
    /* Cascade initialization */
    asm_out8(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    pic_wait();
    asm_out8(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    pic_wait();

    /* Remap */
    asm_out8(PIC1_DATA, PIC1_OFFSET);
    pic_wait();
    asm_out8(PIC2_DATA, PIC2_OFFSET);
    pic_wait();

    /* Cascade identity with slave PIC at IRQ2 */
    asm_out8(PIC1_DATA, 0x04);
    pic_wait();
    asm_out8(PIC2_DATA, 0x02);
    pic_wait();

    /* Request 8086 mode on each PIC */
    asm_out8(PIC1_DATA, 0x01);
    pic_wait();
    asm_out8(PIC2_DATA, 0x01);
    pic_wait();

    asm_out8(PIC1_DATA, 0x00);
    pic_wait();
    asm_out8(PIC2_DATA, 0x00);
    pic_wait();

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
