#pragma once

#include "fmt.h"

IoResult fmt_format(IoWriter writer, Str format, AnyVa args);

IoResult fmt_vprintf(IoWriter writer, Str format, va_list args);

IoResult fmt_printf(IoWriter writer, Str format, ...);

#define fmt_format$(SELF, FMT, ...) \
    fmt_format(io_writer$(SELF), str$(FMT), any_va$(__VA_ARGS__))

#define fmt_print$(FMT, ...) \
    fmt_format(io_chan_out(), str$(FMT), any_va$(__VA_ARGS__))

Str fmt_str(Alloc *alloc, Str format, AnyVa args);

Str fmt_sprintf(Alloc *alloc, Str format, ...);

#define fmt_str$(alloc, format, ...) \
    fmt_str(alloc, str$(format), any_va$(__VA_ARGS__))
