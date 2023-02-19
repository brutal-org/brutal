#include <acpi/rsdt.h>
#include <brutal-debug>

Iter acpi_rsdt_iterate(
    Acpi *acpi,
    IterFn *fn,
    void *ctx)
{
    AcpiRsdt *rsdt = acpi->rsdt;

    size_t child_count = (rsdt->stdh.len - sizeof(rsdt->stdh)) / 4;

    for (size_t i = 0; i < child_count; i++)
    {
        AcpiSdth *sdth = (AcpiSdth *)acpi_phys_to_virt(acpi, rsdt->children[i]);

        if (fn(sdth, ctx) == ITER_STOP)
        {
            return ITER_STOP;
        }
    }

    return ITER_CONTINUE;
}

typedef struct
{
    Str sig;
    IterFn *fn;
    void *ctx;
} LookupCtx;

static Iter lookup_impl(
    AcpiSdth *sdth,
    LookupCtx *ctx)
{
    if (!str_eq(ctx->sig, acpi_sdth_sig(sdth)))
    {
        return ITER_CONTINUE;
    }

    if (ctx->fn(sdth, ctx->ctx) == ITER_STOP)
    {
        return ITER_STOP;
    }

    return ITER_CONTINUE;
}

Iter acpi_rsdt_lookup(
    Acpi *acpi,
    Str sig,
    IterFn *fn,
    void *ctx)
{
    LookupCtx lookup_ctx = {
        sig,
        fn,
        ctx,
    };

    return acpi_rsdt_iterate(
        acpi,
        (IterFn *)lookup_impl,
        &lookup_ctx);
}

typedef struct
{
    Str sig;
    AcpiSdth *sdth;
} LookupFirstCtx;

static Iter lookup_first_impl(
    AcpiSdth *sdth,
    LookupFirstCtx *ctx)
{
    if (!str_eq(ctx->sig, acpi_sdth_sig(sdth)))
    {
        return ITER_CONTINUE;
    }

    ctx->sdth = sdth;

    return ITER_CONTINUE;
}

AcpiSdth *acpi_rsdt_lookup_first(Acpi *acpi, Str sig)
{
    LookupFirstCtx ctx = {sig, nullptr};
    acpi_rsdt_iterate(acpi, (IterFn *)lookup_first_impl, &ctx);
    return ctx.sdth;
}
