#include <embed/log.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <brutal-base>

void embed_log_lock(void)
{
    // FIXME: no-op
}

void embed_log_unlock(void)
{
    // FIXME: no-op
}

static IoResult embed_log_write(MAYBE_UNUSED void *context, uint8_t const *data, size_t size)
{
    if (write(2, data, size) == -1)
    {
        return ERR(IoResult, make_error(ERR_KIND_UNDEFINED, strerror(errno)));
    }

    return OK(IoResult, size);
}

IoWriter embed_log_writer(void)
{
    return (IoWriter){.write = embed_log_write};
}

void embed_log_panic(void)
{
    abort();
}
