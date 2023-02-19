#include <efi/lib.h>
#include <embed/log.h>
#include <brutal-alloc/global.h>
#include <brutal-text/utf16.h>

static IoResult embed_log_write(MAYBE_UNUSED void *context, uint8_t const *data, size_t size)
{
    uint16_t *cstr = utf16_str_to_cstr_dos(str_n$(size, (char *)data), alloc_global());

    efi_st()->console_out->output_string(efi_st()->console_out, cstr);

    alloc_free(alloc_global(), cstr);

    return OK(IoResult, size);
}

IoWriter embed_log_writer(void)
{
    return (IoWriter){.write = embed_log_write};
}

void embed_log_lock(void)
{
    return;
}

void embed_log_unlock(void)
{
    return;
}

void embed_log_panic(void)
{
    while (1)
        ;
}
