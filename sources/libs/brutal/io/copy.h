#pragma once

#include <brutal/io/buf.h>
#include <brutal/io/read.h>
#include <brutal/io/sink.h>
#include <brutal/io/write.h>
#include <brutal/math/clamp.h>

static inline IoResult io_copy(IoReader from, IoWriter to)
{
    size_t total = 0;

    do
    {
        uint8_t buf[512];
        size_t read = TRY(IoResult, io_read(from, buf, 512));

        if (read == 0)
        {
            return OK(IoResult, total);
        }

        size_t written = TRY(IoResult, io_write(to, buf, read));

        if (written == 0)
        {
            return OK(IoResult, total);
        }

        total += written;
    } while (1);
}

static inline IoResult io_copy_n(IoReader from, IoWriter to, size_t size)
{
    size_t total = 0;

    do
    {
        uint8_t buf[512];
        size_t to_read = m_min(512, size - total);
        size_t read = TRY(IoResult, io_read(from, buf, to_read));

        if (read == 0)
        {
            return OK(IoResult, total);
        }

        size_t written = TRY(IoResult, io_write(to, buf, read));

        if (written == 0)
        {
            return OK(IoResult, total);
        }

        total += written;
    } while (total < size);

    return OK(IoResult, total);
}

static inline IoResult io_skip(IoReader from, size_t size)
{
    return io_copy_n(from, io_sink(), size);
}

static inline Buf io_readall(IoReader from, Alloc *alloc)
{
    Buf buf;
    buf_init(&buf, 512, alloc);
    io_copy(from, buf_writer(&buf));
    return buf;
}
