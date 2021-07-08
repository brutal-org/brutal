#pragma once

#include <brutal/base/error.h>
#include <brutal/base/result.h>
#include <brutal/base/std.h>
#include <brutal/text/str.h>

typedef struct io_writer IoWriter;

typedef Result(Error, size_t) IoWriteResult;

typedef IoWriteResult IoWrite(IoWriter *writer, char const *data, size_t size);

struct io_writer
{
    IoWrite *write;
};

#define io_write(writer, data, size) \
    ((writer)->write((writer), (char const *)(data), (size)))

IoWriteResult io_put(IoWriter *writer, char c);

IoWriteResult io_print(IoWriter *writer, Str str);
