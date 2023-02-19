#pragma once

#include <brutal-base>

enum
{
    IDT_TRAP = 0xeF,
    IDT_USER = 0x60,
    IDT_GATE = 0x8e,
};

#define IDT_ENTRY_COUNT (256)

typedef struct PACKED
{
    uint16_t size;
    uint64_t offset;
} IdtDesc;

typedef struct PACKED
{
    uint16_t offset_0_16;
    uint16_t code_segment;
    uint8_t ist;
    uint8_t attributes;
    uint16_t offset_16_32;
    uint32_t offset_32_63;
    uint32_t _zero;
} IdtEntry;

typedef struct PACKED
{
    IdtEntry entries[IDT_ENTRY_COUNT];
} Idt;

IdtEntry idt_entry(uintptr_t handler, uint8_t ist, uint8_t idt_flags);

void idt_initialize(void);

void idt_update(IdtDesc *idt_descriptor);
