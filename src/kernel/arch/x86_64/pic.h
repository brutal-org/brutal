#pragma once

#define PIC1_DATA (0x21)
#define PIC2_DATA (0x29)

void pic_eoi(int int_no);
