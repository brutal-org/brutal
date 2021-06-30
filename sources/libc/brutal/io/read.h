#pragma once

#include <brutal/base/error.h>
#include <brutal/base/result.h>
#include <brutal/base/std.h>

struct reader;

typedef Result(Error, size_t) ReadResult;

typedef ReadResult Read(struct reader *reader, void *data, size_t size);

struct reader
{
    Read *read;
};

#define io_read(reader, data, size) \
    ((reader)->read((reader), (data), (size));
