#pragma once

#include <library/base/error.h>
#include <library/base/std.h>

struct read_result
{
    size_t result;
    struct error error;
};

struct reader;

typedef struct read_result read_t(struct reader *reader, void *data, size_t size);

struct reader
{
    read_t *read;
};

#define io_read(reader, data, size) \
    ((reader)->read((reader), (data), (size));
