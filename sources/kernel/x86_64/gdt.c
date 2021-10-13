#include <brutal/sync.h>
#include "kernel/x86_64/gdt.h"

static struct tss tss = {
    .reserved = 0,
    .rsp = {},
    .reserved0 = 0,
    .ist = {},
    .reserved1 = 0,
    .reserved2 = 0,
    .reserved3 = 0,
    .iopb_offset = 0,
};

static Lock gdt_lock;
static struct gdt gdt = {};

static struct gdt_descriptor gdt_descriptor = {
    .size = sizeof(struct gdt) - 1,
    .offset = (uint64_t)&gdt,
};

struct gdt_entry gdt_entry(uint32_t base, uint32_t limit, uint8_t granularity, uint8_t flags)
{
    return (struct gdt_entry){
        .limit0_15 = (uint16_t)((limit)&0xffff),
        .base0_15 = (uint16_t)((base)&0xffff),
        .base16_23 = (uint8_t)(((base) >> 16) & 0xff),
        .flags = (flags),
        .limit16_19 = ((limit) >> 16) & 0x0f,
        .granularity = (granularity),
        .base24_31 = (uint8_t)(((base) >> 24) & 0xff),
    };
}

struct gdt_entry gdt_entry_null()
{
    return gdt_entry(0, 0, 0, 0);
}

struct gdt_entry gdt_entry_simple(uint8_t flags, uint8_t granularity)
{
    return gdt_entry(0, 0, granularity, flags);
}

struct gdt_tss_entry gdt_entry_tss(uintptr_t tss)
{
    return (struct gdt_tss_entry){
        .length = sizeof(struct tss),
        .base_low16 = tss & 0xffff,
        .base_mid8 = (tss >> 16) & 0xff,
        .flags1 = 0b10001001,
        .flags2 = 0,
        .base_high8 = (tss >> 24) & 0xff,
        .base_upper32 = tss >> 32,
        .reserved = 0,
    };
}

void gdt_initialize(void)
{
    gdt.entries[0] = gdt_entry_null();

    gdt.entries[GDT_KERNEL_CODE] = gdt_entry_simple(GDT_PRESENT | GDT_SEGMENT | GDT_READWRITE | GDT_EXECUTABLE, GDT_LONG_MODE_GRANULARITY);
    gdt.entries[GDT_KERNEL_DATA] = gdt_entry_simple(GDT_PRESENT | GDT_SEGMENT | GDT_READWRITE, 0);

    gdt.entries[GDT_USER_DATA] = gdt_entry_simple(GDT_PRESENT | GDT_SEGMENT | GDT_READWRITE | GDT_USER, 0);
    gdt.entries[GDT_USER_CODE] = gdt_entry_simple(GDT_PRESENT | GDT_SEGMENT | GDT_READWRITE | GDT_EXECUTABLE | GDT_USER, GDT_LONG_MODE_GRANULARITY);

    gdt.tss = gdt_entry_tss((uintptr_t)&tss);

    gdt_update((uintptr_t)&gdt_descriptor);
}

void gdt_load_tss(struct tss *tss)
{
    LOCK_RETAINER(&gdt_lock);
    gdt.tss = gdt_entry_tss((uintptr_t)tss);
    tss_update();
}
