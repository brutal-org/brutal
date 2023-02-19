#include <efi/lib.h>
#include <efi/protos.h>
#include <embed/chan.h>
#include <brutal-alloc>
#include <brutal-debug>
#include <brutal-text>

IoResult embed_chan_read(IoChan channel, uint8_t *data, size_t size)
{
    UNUSED(channel);
    UNUSED(data);
    UNUSED(size);
    panic$("embed_chan_read() not implemented");
}

IoResult embed_chan_write(IoChan channel, uint8_t const *data, size_t size)
{
    assert_not_equal((int)channel, IO_CHAN_IN);

    uint16_t *cstr = utf16_str_to_cstr_dos(str_n$(size, (char *)data), alloc_global());

    efi_st()->console_out->output_string(efi_st()->console_out, cstr);

    alloc_free(alloc_global(), cstr);

    return OK(IoResult, size);
}
