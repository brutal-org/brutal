#pragma once

#include <library/base/error.h>
#include <library/base/result.h>
#include <library/base/std.h>
#include <library/text/str.h>

struct writer;

typedef result_t(size_t) write_r;

typedef write_r write_t(struct writer *writer, const char *data, size_t size);

struct writer
{
    write_t *write;
};

#define io_write(writer, data, size) \
    ((writer)->write((writer), (data), (size)))

write_r io_put(struct writer *writer, char c);

write_r print(struct writer *writer, str_t str);
