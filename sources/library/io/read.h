#pragma once

#include <library/base/error.h>
#include <library/base/result.h>
#include <library/base/std.h>

struct reader;

typedef result_t(error_t, size_t) read_result_t;

typedef read_result_t read_t(struct reader *reader, void *data, size_t size);
struct reader
{
    read_t *read;
};

#define io_read(reader, data, size) \
    ((reader)->read((reader), (data), (size));
