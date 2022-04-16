#pragma once

#include <acpi/base.h>
#include "kernel/cpu.h"

/* --- LApic ---------------------------------------------------------------- */

#define IPI_RESCHED (100)
#define IPI_STOP (101)

#define LAPIC_ENABLE (0x800)

#define LAPIC_SPURIOUS_ALL 0xff
#define LAPIC_SPURIOUS_ENABLE_APIC 0x100

#define LAPIC_ICR_CPUID_OFFSET 24

#define LAPIC_ICR_CLEAR_INIT_LEVEL (1 << 14)

#define LAPIC_ICR_DEST_INIT (5 << 8)
#define LAPIC_ICR_DEST_SEND_IPI (6 << 8)

#define IOAPIC_REG_OFFSET (0)
#define IOAPIC_VALUE_OFFSET (16)

enum lapic_reg
{
    LAPIC_CPU_ID = 0x20,
    LAPIC_REG_EOI = 0x0b0,
    LAPIC_REG_SPURIOUS = 0x0f0,
    LAPIC_REG_ICR0 = 0x300,
    LAPIC_REG_ICR1 = 0x310,
    LAPIC_REG_LVT_TIMER = 0x320,
    LAPIC_REG_TIMER_INITCNT = 0x380,
    LAPIC_REG_TIMER_CURRCNT = 0x390,
    LAPIC_REG_TIMER_DIV = 0x3e0,
};

#define LAPIC_TIMER_IRQ (32)
#define LAPIC_TIMER_PERIODIC (0x20000)
#define LAPIC_TIMER_MASKED (0x10000)

/* Intel Volume 3A 10.5.4 */

enum apic_timer_division
{
    APIC_TIMER_DIVIDE_BY_2 = 0,
    APIC_TIMER_DIVIDE_BY_4 = 1,
    APIC_TIMER_DIVIDE_BY_8 = 2,
    APIC_TIMER_DIVIDE_BY_16 = 3,
    APIC_TIMER_DIVIDE_BY_32 = 4,
    APIC_TIMER_DIVIDE_BY_64 = 5,
    APIC_TIMER_DIVIDE_BY_128 = 6,
    APIC_TIMER_DIVIDE_BY_1 = 7
};

void lapic_timer_initialize(void);

uint32_t lapic_read(uint32_t reg);

void lapic_write(uint32_t reg, uint32_t value);

void lapic_enable(void);

void lapic_enable_spurious(void);

void lapic_eoi(void);

void lapic_send_ipi(int cpu_id, uint32_t interrupt_id);

void lapic_send_init(int cpu_id);

void lapic_send_sipi(int cpu_id, uintptr_t entry);

int lapic_current_cpu(void);

/* --- IoApic --------------------------------------------------------------- */

enum ioapic_reg
{
    IOAPIC_REG_VERSION = 0x1,
    IOAPIC_REG_REDIRECT_BASE = 0x10,
};

typedef struct PACKED
{
    uint8_t version;
    uint8_t reserved;
    uint8_t max_redirect;
    uint8_t reserved2;
} IoapicVersion;

#define IOAPIC_ACTIVE_HIGH_LOW (1 << 1)
#define IOAPIC_TRIGGER_EDGE_LOW (1 << 3)

typedef union PACKED
{
    struct PACKED
    {
        uint8_t interrupt;
        uint8_t delivery_mode : 3;
        uint8_t destination_mode : 1;
        uint8_t delivery_status : 1;
        uint8_t pin_polarity : 1;
        uint8_t remote_irr : 1;
        uint8_t trigger_mode : 1;
        uint8_t mask : 1;
        uint64_t _reserved : 39;
        uint8_t destination;
    };

    struct PACKED
    {
        uint32_t _low_byte;
        uint32_t _high_byte;
    };
} IoapicRedirect;

static_assert(sizeof(IoapicRedirect) == sizeof(uint64_t), "");

/* --- Apic ----------------------------------------------------------------- */

void apic_initalize(Acpi *acpi);
