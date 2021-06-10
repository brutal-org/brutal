#pragma once

#include <brutal/base.h>

#define GDT_ENTRY_COUNT 5
#define GDT_SEGMENT (0b00010000)
#define GDT_PRESENT (0b10000000)
#define GDT_USER (0b01100000)
#define GDT_EXECUTABLE (0b00001000)
#define GDT_READWRITE (0b00000010)
#define GDT_LONG_MODE_GRANULARITY 0b0010
#define GDT_FLAGS 0b1100

struct PACKED tss
{
    uint32_t reserved;

    uint64_t rsp[3];

    uint64_t reserved0;

    uint64_t ist[7];

    uint32_t reserved1;
    uint32_t reserved2;
    uint16_t reserved3;

    uint16_t iopb_offset;
};

struct PACKED gdt_descriptor
{
    uint16_t size;
    uint64_t offset;
};

struct PACKED gdt_entry
{
    uint16_t limit0_15;
    uint16_t base0_15;
    uint8_t base16_23;
    uint8_t flags;
    uint8_t limit16_19 : 4;
    uint8_t granularity : 4;
    uint8_t base24_31;
};

struct PACKED gdt_tss_entry
{
    uint16_t length;
    uint16_t base_low16;
    uint8_t base_mid8;
    uint8_t flags1;
    uint8_t flags2;
    uint8_t base_high8;
    uint32_t base_upper32;
    uint32_t reserved;
};

struct PACKED gdt
{
    struct gdt_entry entries[GDT_ENTRY_COUNT];
    struct gdt_tss_entry tss;
};

struct gdt_entry gdt_entry(uint32_t base, uint32_t limit, uint8_t granularity, uint8_t flags);

struct gdt_entry gdt_entry_simple(uint8_t flags, uint8_t granularity);

struct gdt_tss_entry gdt_entry_tss(uintptr_t tss);

void gdt_update(uint64_t descriptor);

void gdt_initialize(void);
