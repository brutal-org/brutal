#include <embed/chan.h>
#include <embed/posix/err.h>
#include <unistd.h>
#include <brutal-debug>

IoResult embed_chan_read(IoChan channel, uint8_t *data, size_t size)
{
    assert_equal((int)channel, IO_CHAN_IN);

    ssize_t result = read(0, data, size);

    if (result == -1)
    {
        Error error = error_from_errno();
        return ERR(IoResult, error);
    }

    return OK(IoResult, result);
}

IoResult embed_chan_write(IoChan channel, uint8_t const *data, size_t size)
{
    ssize_t result = write((int)channel, data, size);

    if (result == -1)
    {
        Error error = error_from_errno();
        return ERR(IoResult, error);
    }

    return OK(IoResult, result);
}
