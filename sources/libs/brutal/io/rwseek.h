#pragma once

#include <brutal/io/duplex.h>
#include <brutal/io/seek.h>

typedef struct
{
    union
    {
        struct
        {
            IoReader reader;
            IoWriter writer;
        };
        IoDuplex duplex;
    };
    IoSeeker seeker;
} IoRwSeek;

IoResult io_pread(IoRwSeek self, uint8_t *data, size_t size, size_t offset);

IoResult io_pwrite(IoRwSeek self, uint8_t const *data, size_t size, size_t offset);
