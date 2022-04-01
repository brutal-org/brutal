#pragma once

#include <string.h>

#include <libio/Writer.h>
#include <libutils/String.h>

namespace IO
{

static inline ResultOr<size_t> write(Writer &writer, char value)
{
    return writer.write(&value, sizeof(value));
}

static inline ResultOr<size_t> write(Writer &writer, const char *cstring)
{
    return writer.write(cstring, strlen(cstring));
}

static inline ResultOr<size_t> write(Writer &writer, String string)
{
    return writer.write(string.cstring(), string.length());
}

static inline ResultOr<size_t> write(Writer &writer, Slice slice)
{
    return writer.write(slice.start(), slice.size());
}

template <typename T>
static inline ResultOr<size_t> write_struct(Writer &writer, const T &data)
{
    const uint8_t *bytes = reinterpret_cast<const uint8_t *>(&data);
    return writer.write(bytes, sizeof(T));
}

} // namespace IO
