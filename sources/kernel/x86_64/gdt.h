#pragma once

#include <brutal-base>

#define GDT_ENTRY_COUNT 5
#define GDT_SEGMENT (0b00010000)
#define GDT_PRESENT (0b10000000)
#define GDT_USER (0b01100000)
#define GDT_EXECUTABLE (0b00001000)
#define GDT_READWRITE (0b00000010)
#define GDT_LONG_MODE_GRANULARITY 0b0010
#define GDT_FLAGS 0b1100

#define GDT_KERNEL_CODE (1)
#define GDT_KERNEL_DATA (2)
#define GDT_USER_DATA (3)
#define GDT_USER_CODE (4)

#define GDT_RING_3 (3)

typedef struct PACKED
{
    uint32_t reserved;

    uint64_t rsp[3];

    uint64_t reserved0;

    uint64_t ist[7];

    uint32_t reserved1;
    uint32_t reserved2;
    uint16_t reserved3;

    uint16_t iopb_offset;
} Tss;

typedef struct PACKED
{
    uint16_t size;
    uint64_t offset;
} GdtDesc;

typedef struct PACKED
{
    uint16_t limit0_15;
    uint16_t base0_15;
    uint8_t base16_23;
    uint8_t flags;
    uint8_t limit16_19 : 4;
    uint8_t granularity : 4;
    uint8_t base24_31;
} GdtEntry;

typedef struct PACKED
{
    uint16_t len;
    uint16_t base_low16;
    uint8_t base_mid8;
    uint8_t flags1;
    uint8_t flags2;
    uint8_t base_high8;
    uint32_t base_upper32;
    uint32_t reserved;
} GdtTssEntry;

typedef struct PACKED
{
    GdtEntry entries[GDT_ENTRY_COUNT];
    GdtTssEntry tss;
} Gdt;

GdtEntry gdt_entry(uint32_t base, uint32_t limit, uint8_t granularity, uint8_t flags);

GdtEntry gdt_entry_null(void);

GdtEntry gdt_entry_simple(uint8_t flags, uint8_t granularity);

GdtTssEntry gdt_entry_tss(uintptr_t tss);

void gdt_update(uint64_t descriptor);

void gdt_initialize(void);

void gdt_load_tss(Tss *tss);

void tss_update(void);
