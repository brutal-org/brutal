#include "kernel/x86_64/gdt.h"
#include <brutal-sync>

static Tss _tss = {
    .reserved = 0,
    .rsp = {},
    .reserved0 = 0,
    .ist = {},
    .reserved1 = 0,
    .reserved2 = 0,
    .reserved3 = 0,
    .iopb_offset = 0,
};

static Lock _lock;
static Gdt _gdt = {};

static GdtDesc _gdt_desc = {
    .size = sizeof(Gdt) - 1,
    .offset = (uint64_t)&_gdt,
};

GdtEntry gdt_entry(uint32_t base, uint32_t limit, uint8_t granularity, uint8_t flags)
{
    return (GdtEntry){
        .limit0_15 = (uint16_t)((limit)&0xffff),
        .base0_15 = (uint16_t)((base)&0xffff),
        .base16_23 = (uint8_t)(((base) >> 16) & 0xff),
        .flags = (flags),
        .limit16_19 = ((limit) >> 16) & 0x0f,
        .granularity = (granularity),
        .base24_31 = (uint8_t)(((base) >> 24) & 0xff),
    };
}

GdtEntry gdt_entry_null()
{
    return gdt_entry(0, 0, 0, 0);
}

GdtEntry gdt_entry_simple(uint8_t flags, uint8_t granularity)
{
    return gdt_entry(0, 0, granularity, flags);
}

GdtTssEntry gdt_entry_tss(uintptr_t tss_addr)
{
    return (GdtTssEntry){
        .len = sizeof(Tss),
        .base_low16 = tss_addr & 0xffff,
        .base_mid8 = (tss_addr >> 16) & 0xff,
        .flags1 = 0b10001001,
        .flags2 = 0,
        .base_high8 = (tss_addr >> 24) & 0xff,
        .base_upper32 = tss_addr >> 32,
        .reserved = 0,
    };
}

void gdt_initialize(void)
{
    _gdt.entries[0] = gdt_entry_null();

    _gdt.entries[GDT_KERNEL_CODE] = gdt_entry_simple(GDT_PRESENT | GDT_SEGMENT | GDT_READWRITE | GDT_EXECUTABLE, GDT_LONG_MODE_GRANULARITY);
    _gdt.entries[GDT_KERNEL_DATA] = gdt_entry_simple(GDT_PRESENT | GDT_SEGMENT | GDT_READWRITE, 0);

    _gdt.entries[GDT_USER_DATA] = gdt_entry_simple(GDT_PRESENT | GDT_SEGMENT | GDT_READWRITE | GDT_USER, 0);
    _gdt.entries[GDT_USER_CODE] = gdt_entry_simple(GDT_PRESENT | GDT_SEGMENT | GDT_READWRITE | GDT_EXECUTABLE | GDT_USER, GDT_LONG_MODE_GRANULARITY);

    _gdt.tss = gdt_entry_tss((uintptr_t)&_tss);

    gdt_update((uintptr_t)&_gdt_desc);
}

void gdt_load_tss(Tss *tss)
{
    LOCK_RETAINER(&_lock);
    _gdt.tss = gdt_entry_tss((uintptr_t)tss);
    tss_update();
}
