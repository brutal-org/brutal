#include <bal/abi.h>
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

    br_log(&(BrLogArgs){
        .message = (char const *)data,
        .size = size,
    });

    return OK(IoResult, size);
}
