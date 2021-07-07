#pragma once

#include <brutal/base/error.h>
#include <brutal/base/result.h>
#include <brutal/base/std.h>
#include <brutal/text/str.h>

struct writer;

typedef Result(Error, size_t) WriteResult;

typedef WriteResult Write(struct writer *writer, char const *data, size_t size);

struct writer
{
    Write *write;
};

#define io_write(writer, data, size) \
    ((writer)->write((writer), (char const *)(data), (size)))

WriteResult io_put(struct writer *writer, char c);

WriteResult print(struct writer *writer, Str str);
