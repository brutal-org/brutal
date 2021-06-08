#pragma once

#include <library/base.h>

#include "kernel/cpu.h"
#include "kernel/handover.h"

#define LAPIC_ENABLE (0x800)

#define LAPIC_SPURIOUS_ALL 0xff
#define LAPIC_SPURIOUS_ENABLE_APIC 0x100

#define LAPIC_ICR_CPUID_OFFSET 24

#define LAPIC_ICR_CLEAR_INIT_LEVEL (1 << 14)

#define LAPIC_ICR_DEST_INIT (5 << 8)
#define LAPIC_ICR_DEST_SEND_IPI (6 << 8)

enum lapic_reg
{
    LAPIC_CPU_ID = 0x20,
    LAPIC_REG_EOI = 0x0b0,
    LAPIC_REG_SPURIOUS = 0x0f0,
    LAPIC_REG_ICR0 = 0x300,
    LAPIC_REG_ICR1 = 0x310,
};

enum ioapic_reg
{
    IOAPIC_REG_VERSION = 0x1,
};

struct PACKED ioapic_version
{
    uint8_t version;
    uint8_t reserved;
    uint8_t maximum_redirection;
    uint8_t reserved2;
};

void apic_enable(void);

void apic_initalize(struct handover const *handover);

void lapic_enable_spurious(void);

void apic_eoi(void);

void apic_send_ipit(cpu_id_t cpu_id, uint32_t interrupt_id);

void apic_init_processor(cpu_id_t cpu_id);

void apic_start_processor(cpu_id_t cpu_id, uintptr_t entry);

cpu_id_t apic_current_cpu(void);

struct ioapic_version ioapic_get_version(int ioapic_id);