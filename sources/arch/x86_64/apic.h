#pragma once

#include <brutal/base.h>
#include "arch/cpu.h"
#include "kernel/handover.h"

#define IPIT_RESCHED (100)
#define IPIT_STOP (101)

#define LAPIC_ENABLE (0x800)

#define LAPIC_SPURIOUS_ALL 0xff
#define LAPIC_SPURIOUS_ENABLE_APIC 0x100

#define LAPIC_ICR_CPUID_OFFSET 24

#define LAPIC_ICR_CLEAR_INIT_LEVEL (1 << 14)

#define LAPIC_ICR_DEST_INIT (5 << 8)
#define LAPIC_ICR_DEST_SEND_IPI (6 << 8)

#define IOAPIC_REG_OFFSET (0)
#define IOAPIC_VALUE_OFFSET (16)

typedef result_t(int, int) ioapic_int_redirect_result_t;

#define LAPIC_REGISTER_LVT_INT_MASKED 0x10000

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

enum ioapic_reg
{
    IOAPIC_REG_VERSION = 0x1,
    IOAPIC_REG_REDIRECT_BASE = 0x10,
};

struct PACKED ioapic_version
{
    uint8_t version;
    uint8_t reserved;
    uint8_t maximum_redirection;
    uint8_t reserved2;
};

enum PACKED ioapic_ipit_flags
{
    IPIT_FLAGS_ACTIVE_HIGH_LOW = 1 << 1,
    IPIT_EDGE_LOW = 1 << 3
};

struct PACKED ioapic_ipit_redirection_entry
{
    union
    {

        struct PACKED
        {
            uint64_t interrupt : 8;
            uint64_t delivery_mode : 3;
            uint64_t destination_mode : 1;
            uint64_t delivery_status : 1;
            uint64_t pin_polarity : 1;
            uint64_t remote_irr : 1;
            uint64_t trigger_mode : 1;
            uint64_t mask : 1;
            uint64_t _reserved : 39;
            uint64_t destination : 8;
        };

        struct PACKED
        {
            uint32_t _low_byte;
            uint32_t _high_byte;
        };
    };
};

uint32_t lapic_read(uint32_t reg);

void lapic_write(uint32_t reg, uint32_t value);

void apic_enable(void);

void apic_initalize(struct handover const *handover);

void lapic_enable_spurious(void);

void apic_eoi(void);

void apic_send_ipit(cpu_id_t cpu_id, uint32_t interrupt_id);

void apic_init_processor(cpu_id_t cpu_id);

void apic_start_processor(cpu_id_t cpu_id, uintptr_t entry);

cpu_id_t apic_current_cpu(void);

struct ioapic_version ioapic_get_version(int ioapic_id);

ioapic_int_redirect_result_t apic_redirect_irq_to_cpu(cpu_id_t id, uint8_t irq, bool enable, uintptr_t rsdp);

ioapic_int_redirect_result_t apic_init_interrupt_redirection(struct handover const *handover);
