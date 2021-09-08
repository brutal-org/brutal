#pragma once

#define PACKED __attribute__((packed))

#define ALIGNED(ALIGN) __attribute__((aligned(ALIGN)))

#define UNUSED(EXPR) ((void)(EXPR))

#define MAYBE_UNUSED [[maybe_unused]]

#define CLEANUP(FUNC) __attribute__((cleanup(FUNC)))

#define WEAK __attribute__((weak))