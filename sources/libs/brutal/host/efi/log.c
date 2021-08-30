#include <brutal/alloc/global.h>
#include <brutal/host/log.h>
#include <brutal/text/utf16.h>
#include <efi/lib.h>

static IoWriter writer;

static IoWriteResult host_log_write(MAYBE_UNUSED void *context, uint8_t const *data, MAYBE_UNUSED size_t offset, size_t size)
{

    uint16_t *cstr = str_to_cstr_utf16_dos(str_n$(size, (char *)data), alloc_global());

    efi_st()->console_out->output_string(efi_st()->console_out, cstr);

    alloc_free(alloc_global(), cstr);

    return OK(IoWriteResult, size);
}

IoWriter *host_log_writer(void)
{

    writer.write = host_log_write;

    return &writer;
}

void host_log_lock(void)
{
    return;
}

void host_log_unlock(void)
{
    return;
}

void host_log_panic(void)
{
    while (1)
        ;
}
