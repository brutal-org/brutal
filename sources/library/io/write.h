#pragma once

#include <library/base/error.h>
#include <library/base/std.h>
#include <library/base/str.h>

struct write_result
{
    struct error error;
    size_t written;
};

static inline bool is_write_err(struct write_result res)
{
    return res.error.kind != ERR_SUCCESS;
}

static inline struct write_result write_err(struct error err)
{
    return (struct write_result){.error = err};
}

static inline struct write_result write_ok(size_t ok)
{
    return (struct write_result){.written = ok};
}

struct writer;

typedef struct write_result write_t(struct writer *writer, const char *data, size_t size);

struct writer
{
    write_t *write;
};

#define io_write(writer, data, size) \
    ((writer)->write((writer), (data), (size)))

struct write_result io_put(struct writer *writer, char c);

struct write_result print(struct writer *writer, str_t str);
