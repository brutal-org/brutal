#pragma once

#include <brutal/base/error.h>
#include <brutal/base/result.h>
#include <brutal/base/std.h>

struct reader;

typedef Result(Error, size_t) IoReadResult;

typedef IoReadResult IORead(struct reader *reader, void *data, size_t size);

typedef struct reader
{
    IORead *read;
} IOReader;

#define io_read(reader, data, size) \
    ((reader)->read((reader), (data), (size));
