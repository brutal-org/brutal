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

struct PACKED interrupt_stackframe
{
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t r11;
    uint64_t r10;
    uint64_t r9;
    uint64_t r8;
    uint64_t rbp;
    uint64_t rdi;
    uint64_t rsi;
    uint64_t rdx;
    uint64_t rcx;
    uint64_t rbx;
    uint64_t rax;

    uint64_t int_no;
    uint64_t error_code;

    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
};

struct idt_entry idt_entry(uintptr_t handler, uint8_t ist, uint8_t idt_flags);

void idt_initialize(void);

void idt_update(struct idt_descriptor *idt_descriptor);