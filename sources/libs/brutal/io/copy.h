#pragma once

#include <brutal/io/buf.h>
#include <brutal/io/read.h>
#include <brutal/io/write.h>

typedef Result(Error, size_t) IoCopyResult;

static inline IoCopyResult io_copy(IoReader *from, IoWriter *to)
{
    size_t total = 0;

    do
    {
        uint8_t buf[512];
        size_t read = TRY(IoCopyResult, io_read(from, buf, 512));

        if (read == 0)
        {
            return OK(IoCopyResult, total);
        }

        size_t written = TRY(IoCopyResult, io_write(to, buf, read));

        if (written == 0)
        {
            return OK(IoCopyResult, total);
        }

        total += written;
    } while (1);
}

static inline Buf io_readall(IoReader *from, Alloc *alloc)
{
    Buf buf;
    buf_init(&buf, 512, alloc);
    IoWriter to = buf_writer(&buf);
    io_copy(from, &to);
    return buf;
}
