#pragma once

#include <library/base.h>

enum idt_entry_flags
{
    IDT_TRAP = 0xeF,
    IDT_USER = 0x60,
    IDT_GATE = 0x8e,
};

#define IDT_ENTRY_COUNT (256)

struct PACKED idt_descriptor
{
    uint16_t size;
    uint64_t offset;
};

struct PACKED idt_entry
{
    uint16_t offset_0_16;
    uint16_t code_segment;
    uint8_t ist;
    uint8_t attributes;
    uint16_t offset_16_32;
    uint32_t offset_32_63;
    uint32_t _zero;
};

struct PACKED idt
{
    struct idt_entry entries[IDT_ENTRY_COUNT];
};

struct idt_entry idt_entry(uintptr_t handler, uint8_t ist, uint8_t idt_flags);

void idt_initialize(void);

void idt_update(struct idt_descriptor *idt_descriptor);