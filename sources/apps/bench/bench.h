#pragma once

#include <brutal/debug.h>
#include <brutal/gfx.h>

typedef void BenchFunc(Gfx *self, int tick);

typedef struct
{
    Str name;
    BenchFunc *func;
} Bench;

void bench_alloc_begin_test(void);

void bench_alloc_end_test(void);

void bench_register(Bench bench);

#define BENCH_FUNCTION_NAME(name) bench_##name

#define BENCH(name)                                                              \
    void BENCH_FUNCTION_NAME(name)(Gfx * gfx, int tick);                         \
    [[gnu::constructor]] static inline void bench_register_##name##_detail(void) \
    {                                                                            \
        bench_register((Bench){                                                  \
            str$(#name),                                                         \
            BENCH_FUNCTION_NAME(name),                                           \
        });                                                                      \
    }                                                                            \
    void BENCH_FUNCTION_NAME(name)(MAYBE_UNUSED Gfx * gfx, MAYBE_UNUSED int tick)
