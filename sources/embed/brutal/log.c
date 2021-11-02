#include <bal/abi.h>
#include <brutal/alloc/global.h>
#include <brutal/io/buf.h>
#include <brutal/sync.h>
#include <embed/log.h>

static bool initialized = false;
static Lock lock = {};
static Buf buf = {};
static IoWriter log = {};

void host_log_lock(void)
{
    lock_acquire(&lock);
}

void host_log_unlock(void)
{
    lock_release(&lock);
}

static IoResult host_log_write(MAYBE_UNUSED void *context, uint8_t const *data, MAYBE_UNUSED size_t offset, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        buf_push(&buf, data[i]);

        if (data[i] == '\n')
        {
            br_log(&(BrLogArgs){
                .message = (char const *)buf_begin(&buf),
                .size = buf_used(&buf),
            });

            buf_clear(&buf);
        }
    }

    return OK(IoResult, size);
}

IoWriter *host_log_writer(void)
{
    if (!initialized)
    {
        log.write = host_log_write;
        buf_init(&buf, 128, alloc_global());
        initialized = true;
    }

    return &log;
}

void host_log_panic(void)
{
    br_exit(&(BrExitArgs){
        .task = BR_TASK_SELF,
        .exit_value = -1,
    });

    __builtin_unreachable();
}
