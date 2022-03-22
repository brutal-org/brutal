#pragma once

#include <brutal/base/std.h>
#include <brutal/io/fmt.h>

Str str_fmt(Alloc *alloc, Str format, AnyVa args);

#define str_fmt$(alloc, fmt, ...) \
    str_fmt(alloc, str$(fmt), any_va$(__VA_ARGS__))
