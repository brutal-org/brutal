#pragma once

#include <brutal/io/funcs.h>
#include <brutal/base/any.h>
#include <brutal/base/str.h>

IoResult cli_fmt(IoWriter writer, Str format, AnyVa args);

#define cli_fmt$(WRITER, FMT, ...) \
    cli_fmt(io_writer$(WRITER), str$(FMT), any_va$(__VA_ARGS__))
