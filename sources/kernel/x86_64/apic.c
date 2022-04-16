#include <acpi/madt.h>
#include <acpi/rsdt.h>
#include <brutal-debug>
#include <brutal-mem>
#include "kernel/mmap.h"
#include "kernel/x86_64/apic.h"
#include "kernel/x86_64/asm.h"
#include "kernel/x86_64/cpu.h"
#include "kernel/x86_64/hpet.h"
#include "kernel/x86_64/pic.h"

/* --- Lapic ---------------------------------------------------------------- */

static uintptr_t lapic_base = 0;

uint32_t lapic_read(uint32_t reg)
{
    return volatile_read32((uintptr_t)lapic_base + reg);
}

void lapic_write(uint32_t reg, uint32_t value)
{
    volatile_write32(lapic_base + reg, value);
}

void lapic_eoi(void)
{
    lapic_write(LAPIC_REG_EOI, 0);
}

void lapic_send_ipi(int cpu_id, uint32_t interrupt_id)
{
    lapic_write(LAPIC_REG_ICR1, (uint64_t)cpu_id << LAPIC_ICR_CPUID_OFFSET);
    lapic_write(LAPIC_REG_ICR0, interrupt_id | LAPIC_ICR_CLEAR_INIT_LEVEL);
}

void lapic_send_init(int cpu_id)
{
    lapic_write(LAPIC_REG_ICR1, (uint64_t)cpu_id << LAPIC_ICR_CPUID_OFFSET);
    lapic_write(LAPIC_REG_ICR0, LAPIC_ICR_DEST_INIT);
}

void lapic_send_sipi(int cpu_id, uintptr_t entry)
{
    lapic_write(LAPIC_REG_ICR1, (uint64_t)cpu_id << LAPIC_ICR_CPUID_OFFSET);
    lapic_write(LAPIC_REG_ICR0, LAPIC_ICR_DEST_SEND_IPI | (entry / 4096));
}

int lapic_current_cpu(void)
{
    if (lapic_base == 0)
    {
        return 0;
    }
    else
    {
        return lapic_read(LAPIC_CPU_ID) >> 24;
    }
}

void lapic_enable(void)
{
    asm_write_msr(MSR_APIC, (asm_read_msr(MSR_APIC) | LAPIC_ENABLE) & ~((1 << 10)));

    lapic_write(LAPIC_REG_SPURIOUS, lapic_read(LAPIC_REG_SPURIOUS) | (LAPIC_SPURIOUS_ALL | LAPIC_SPURIOUS_ENABLE_APIC));
}

void lapic_timer_initialize(void)
{
    lapic_write(LAPIC_REG_TIMER_DIV, APIC_TIMER_DIVIDE_BY_16);
    lapic_write(LAPIC_REG_TIMER_INITCNT, 0xFFFFFFFF); /* Set the value to -1 */

    hpet_sleep(10);

    lapic_write(LAPIC_REG_LVT_TIMER, LAPIC_TIMER_MASKED);

    uint32_t tick_in_10ms = 0xFFFFFFFF - lapic_read(LAPIC_REG_TIMER_CURRCNT);

    lapic_write(LAPIC_REG_LVT_TIMER, LAPIC_TIMER_IRQ | LAPIC_TIMER_PERIODIC);
    lapic_write(LAPIC_REG_TIMER_DIV, APIC_TIMER_DIVIDE_BY_16);
    lapic_write(LAPIC_REG_TIMER_INITCNT, tick_in_10ms / 10);
}

static Iter lapic_iterate(AcpiMadtLapicRecord *lapic, MAYBE_UNUSED void *ctx)
{
    cpu_found(lapic->processor_id, lapic->id);
    return ITER_CONTINUE;
}

/* --- Ioapic --------------------------------------------------------------- */

#define IOAPIC_MAX_COUNT (256)

typedef struct
{
    uintptr_t base;
    uint32_t interrupt_base;
    uint32_t max_redirect;
} Ioapic;

static int _ioapic_count = 0;
static Ioapic _ioapic[IOAPIC_MAX_COUNT] = {};

static inline uint32_t ioapic_read(int index, uint32_t reg)
{
    volatile_write32(_ioapic[index].base + IOAPIC_REG_OFFSET, reg);
    return volatile_read32(_ioapic[index].base + IOAPIC_VALUE_OFFSET);
}

static inline void ioapic_write(int index, uint32_t reg, uint32_t value)
{
    volatile_write32(_ioapic[index].base + IOAPIC_REG_OFFSET, reg);
    volatile_write32(_ioapic[index].base + IOAPIC_VALUE_OFFSET, value);
}

static IoapicVersion ioapic_version(int index)
{
    uint32_t raw = ioapic_read(index, IOAPIC_REG_VERSION);
    return union$(IoapicVersion, raw);
}

static int ioapic_from_gsi(uint32_t interrupt_base)
{
    for (int i = 0; i < _ioapic_count; i++)
    {
        U8Range interrupt_range = {
            .base = _ioapic[i].interrupt_base,
            .size = _ioapic[i].max_redirect,
        };

        if (range_contain(interrupt_range, interrupt_base))
        {
            return i;
        }
    }

    panic$("Ioapic interrupt base not found for base {}", interrupt_base);
}

static void ioapic_create_redirect(
    uint8_t interrupt_id,
    uint32_t interrupt_base,
    uint16_t flags,
    int cpu,
    bool enable)
{
    int ioapic_target = ioapic_from_gsi(interrupt_base);

    IoapicRedirect entry = {};

    entry.interrupt = interrupt_id;

    if (flags & IOAPIC_ACTIVE_HIGH_LOW)
    {
        entry.pin_polarity = 1;
    }

    if (flags & IOAPIC_TRIGGER_EDGE_LOW)
    {
        entry.trigger_mode = 1;
    }

    if (!enable)
    {
        entry.mask = 1;
    }

    entry.destination = cpu_impl(cpu)->lapic;

    uint32_t interrupt_offset = interrupt_base - _ioapic[ioapic_target].interrupt_base;

    uint32_t target_io_offset = interrupt_offset * 2;

    ioapic_write(ioapic_target, IOAPIC_REG_REDIRECT_BASE + target_io_offset, entry._low_byte);
    ioapic_write(ioapic_target, IOAPIC_REG_REDIRECT_BASE + target_io_offset + 1, entry._high_byte);
}

typedef struct
{
    int id;
    uint8_t irq;
    bool enable;
} IrqRedirectCtx;

static Iter iso_iterate(AcpiMadtIsoRecord *record, IrqRedirectCtx *ctx)
{
    if (record->irq == ctx->irq)
    {
        log$("   - Using iso interrupt base: {}", record->gsi);

        ioapic_create_redirect(
            record->irq + 0x20,
            record->gsi,
            record->flags,
            ctx->id,
            ctx->enable);

        return ITER_STOP;
    }

    return ITER_CONTINUE;
}

static void ioapic_redirect_irq_to_cpu(Acpi *acpi, int id, uint8_t irq, bool enable)
{
    log$(" - Setting ipi {#p} redirection for cpu: {}", irq, id);

    IrqRedirectCtx ctx = {id, irq, enable};

    if (acpi_madt_lookup(
            acpi,
            ACPI_MADT_RECORD_ISO,
            (IterFn *)iso_iterate,
            &ctx) == ITER_STOP)
    {
        return;
    }

    ioapic_create_redirect(irq + 0x20, irq, 0, id, enable);
}

static void ioapic_redirect_legacy_irq(Acpi *acpi)
{
    log$("IOApic: Setup irq redirection");

    for (size_t i = 0; i < 16; i++) // set interrupt 32 to 48
    {
        ioapic_redirect_irq_to_cpu(acpi, lapic_current_cpu(), i, true);
    }
}

static Iter ioapic_iterate(AcpiMadtIoapicRecord *ioapic, MAYBE_UNUSED void *ctx)
{
    int index = _ioapic_count++;
    _ioapic[index].base = mmap_phys_to_io(ioapic->address);
    _ioapic[index].interrupt_base = ioapic->interrupt_base;
    _ioapic[index].max_redirect = ioapic_version(index).max_redirect;

    return ITER_CONTINUE;
}

/* --- Apic ----------------------------------------------------------------- */

void apic_initalize(Acpi *acpi)
{
    // Setup local apics
    AcpiMadt *madt = (AcpiMadt *)acpi_rsdt_lookup_first(acpi, ACPI_MADT_SIG);

    lapic_base = mmap_phys_to_io(madt->lapic);

    acpi_madt_lookup(
        acpi,
        ACPI_MADT_RECORD_LAPIC,
        (IterFn *)lapic_iterate,
        nullptr);

    // Setup io apics
    acpi_madt_lookup(
        acpi,
        ACPI_MADT_RECORD_IOAPIC,
        (IterFn *)ioapic_iterate,
        nullptr);

    lapic_enable();

    ioapic_redirect_legacy_irq(acpi);
    lapic_timer_initialize();
}
