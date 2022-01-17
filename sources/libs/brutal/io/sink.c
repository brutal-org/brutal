#include <brutal/base/attributes.h>
#include <brutal/io/write.h>

static IoResult io_sink_write(MAYBE_UNUSED void *ctx, MAYBE_UNUSED uint8_t const *buf, size_t size)
{
    return OK(IoResult, size);
}

IoWriter io_sink(void)
{
    return (IoWriter){
        .write = io_sink_write,
    };
}
