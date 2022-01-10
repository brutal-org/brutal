#pragma once

#include <brutal/base/types.h>
#include <brutal/io/base.h>

typedef enum
{
    IO_WHENCE_START,
    IO_WHENCE_CURRENT,
    IO_WHENCE_END,
} IoWhence;

typedef struct
{
    IoWhence whence;
    SSize position;
} IoSeek;

IoSeek io_seek_from_start(SSize position);

IoSeek io_seek_from_current(SSize position);

IoSeek io_seek_from_end(SSize position);

typedef IoResult IoSeekFn(void *context, IoSeek off);

typedef struct
{
    IoSeekFn *seek;
    void *context;
} IoSeeker;

IoResult io_seek(IoSeeker self, IoSeek off);
