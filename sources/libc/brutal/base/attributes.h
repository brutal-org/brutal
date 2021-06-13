#pragma once

#define PACKED [[gnu::packed]]

#define ALIGNED(__align) [[gnu::aligned(__align)]]

#define UNUSED(expr) ((void)expr)

#define MAYBE_UNUSED [[maybe_unused]]

#define CLEANUP(func) [[gnu::cleanup(func)]]
