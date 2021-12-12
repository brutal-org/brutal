#include <brutal/base.h>
#include <embed/log.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static bool _init = false;
static IoWriter _log;

void embed_log_lock(void)
{
    // FIXME: no-op
}

void embed_log_unlock(void)
{
    // FIXME: no-op
}

static IoResult embed_log_write(MAYBE_UNUSED void *context, uint8_t const *data, MAYBE_UNUSED size_t offset, size_t size)
{
    if (write(2, data, size) == -1)
    {
        return ERR(IoResult, make_error(ERR_KIND_UNDEFINED, strerror(errno)));
    }

    return OK(IoResult, size);
}

IoWriter *embed_log_writer(void)
{
    if (!_init)
    {
        _log.write = embed_log_write;
        _init = true;
    }

    return &_log;
}

void embed_log_panic(void)
{
    abort();
}
