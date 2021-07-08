#pragma once

#include <brutal/base/error.h>
#include <brutal/base/result.h>
#include <brutal/base/std.h>

typedef struct io_reader IoReader;

typedef Result(Error, size_t) IoReadResult;

typedef IoReadResult IoRead(IoReader *reader, char *data, size_t size);

struct io_reader
{
    IoRead *read;
};

#define io_read(reader, data, size) \
    ((reader)->read((reader), (data), (size));
