#pragma once

#include <bruwutal/base/std.h>
#include <bruwutal/io/fmt.h>

Str str_fmt(Alloc *alloc, Str format, AnyVa args);

#define str_fmt$(alloc, fmt, ...) \
    str_fmt(alloc, str$(fmt), any_va$(__VA_ARGS__))
