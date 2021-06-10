#pragma once

#include <brutal/base/error.h>
#include <brutal/base/result.h>
#include <brutal/base/std.h>
#include <brutal/text/str.h>

struct writer;

typedef result_t(error_t, size_t) write_result_t;

typedef write_result_t write_t(struct writer *writer, char const *data, size_t size);

struct writer
{
    write_t *write;
};

#define io_write(writer, data, size) \
    ((writer)->write((writer), (data), (size)))

write_result_t io_put(struct writer *writer, char c);

write_result_t print(struct writer *writer, str_t str);
