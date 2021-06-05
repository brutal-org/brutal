#pragma once

#include <library/base/error.h>
#include <library/base/result.h>
#include <library/base/std.h>

struct reader;

typedef result_t(size_t) read_r;

typedef read_r read_t(struct reader *reader, void *data, size_t size);

struct reader
{
    read_t *read;
};

#define io_read(reader, data, size) \
    ((reader)->read((reader), (data), (size));
