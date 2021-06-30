#pragma once

#define PACKED [[gnu::packed]]

#define ALIGNED(ALIGN) [[gnu::aligned(ALIGN)]]

#define MAYBE_UNUSED [[maybe_unused]]

#define CLEANUP(func) [[gnu::cleanup(func)]]
