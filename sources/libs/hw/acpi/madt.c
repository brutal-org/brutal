#include <acpi/madt.h>
#include <acpi/rsdt.h>

Iter acpi_madt_iterate(Acpi *acpi, IterFn *fn, void *ctx)
{
    AcpiMadt *madt = (AcpiMadt *)acpi_rsdt_lookup_first(acpi, ACPI_MADT_SIG);

    if (!madt)
    {
        return ITER_CONTINUE;
    }

    AcpiMadtRecord *record = madt->records;

    while ((uintptr_t)record < (uintptr_t)madt + madt->sdth.len)
    {
        if (fn(record, ctx) == ITER_STOP)
        {
            return ITER_STOP;
        }

        record = (AcpiMadtRecord *)(((uintptr_t)record) + record->len);
    }

    return ITER_CONTINUE;
}

typedef struct
{
    AcpiMadtRecordType type;
    IterFn *fn;
    void *ctx;
} LookupCtx;

static Iter lookup_impl(AcpiMadtRecord *record, LookupCtx *ctx)
{
    if (record->type != ctx->type)
    {
        return ITER_CONTINUE;
    }

    if (ctx->fn(record, ctx->ctx) == ITER_STOP)
    {
        return ITER_STOP;
    }

    return ITER_CONTINUE;
}

Iter acpi_madt_lookup(Acpi *acpi, AcpiMadtRecordType type, IterFn *fn, void *ctx)
{
    LookupCtx lookup_ctx = {type, fn, ctx};
    return acpi_madt_iterate(acpi, (IterFn *)lookup_impl, &lookup_ctx);
}
