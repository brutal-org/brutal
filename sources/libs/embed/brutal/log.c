#include <bal/abi.h>
#include <embed/log.h>
#include <brutal-alloc/global.h>
#include <brutal-io/buf.h>
#include <brutal-sync>

static bool _initialized = false;
static Lock _lock;
static Buf _buf = {};

void embed_log_lock(void)
{
    lock_acquire(&_lock);
}

void embed_log_unlock(void)
{
    lock_release(&_lock);
}

static IoResult embed_log_write(MAYBE_UNUSED void *context, uint8_t const *data, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        buf_push(&_buf, data[i]);

        if (data[i] == '\n')
        {
            br_log(&(BrLogArgs){
                .message = (char const *)buf_begin(&_buf),
                .size = buf_used(&_buf),
            });

            buf_clear(&_buf);
        }
    }

    return OK(IoResult, size);
}

IoWriter embed_log_writer(void)
{
    if (!_initialized)
    {
        buf_init(&_buf, 128, alloc_global());
        _initialized = true;
    }

    return (IoWriter){.write = embed_log_write};
}

void embed_log_panic(void)
{
    br_exit(&(BrExitArgs){
        .handle = BR_HANDLE_SELF,
        .result = -1,
    });

    __builtin_unreachable();
}
