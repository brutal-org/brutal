#pragma once

#define PACKED __attribute__((packed))

#define FLATTEN __attribute__((flatten))

#define ALIGNED(ALIGN) __attribute__((aligned(ALIGN)))

#define UNUSED(EXPR) ((void)(EXPR))

#define MAYBE_UNUSED [[maybe_unused]]

#define CLEANUP(FUNC) __attribute__((cleanup(FUNC)))

#define WEAK __attribute__((weak))

#define LIKELY(EXPR) __builtin_expect((bool)(EXPR), true)

#define UNLIKELY(EXPR) __builtin_expect((bool)(EXPR), false)
