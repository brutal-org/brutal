#pragma once

#include <math.h>

#if defined(__x86_64__) && !defined(NO_FAST_MATH_LIB)
#    include <immintrin.h>
#    include <x86intrin.h>

/**
    this function is tested, from [0, 100] with a step of 0.1. And the result is always:
    sqrt(x) - 0.01f <= result <= sqrt(x) + 0.01f
*/
static inline float fast_sqrt(float number)
{
    float a;
    float b = number;
    __m128 in = _mm_load_ss(&b);
    _mm_store_ss(&a, _mm_sqrt_ss(in));
    return a;
}

/**
    this function is tested, from [0, 100] with a step of 0.1. And the result is always:
    1/sqrt(x) - 0.01f <= result <= 1/sqrt(x) + 0.01f
*/
static inline float fast_invert_sqrt(float number)
{
    float a;
    float b = number;
    __m128 in = _mm_load_ss(&b);
    _mm_store_ss(&a, _mm_rsqrt_ss(in));
    return a;
}

#else

static inline float fast_sqrt(float number)
{
    return sqrt(number);
}

static inline float fast_invert_rsqrt(float number)
{
    return 1 / sqrt(number);
}

#endif

#if !defined(NO_FAST_MATH_LIB)
/**
    this function is tested, with y in [-50, 50] and x in [-50, 50] with a step of 0.1. And the result is always:
    atan2(x,y) - 0.02f <= result <= atan2(x, y) + 0.02f
*/
static inline float fast_atan2(float y, float x)
{
    float abs_y = fabs(y) + 0.0000000001;
    float r = (x - __builtin_copysign(abs_y, x)) / (abs_y + fabs(x));
    float angle = (M_PI_2)-__builtin_copysign(M_PI_4, x);

    angle += (0.1963f * r * r - 0.9817f) * r;
    return __builtin_copysign(angle, y);
}

/**
    this function is tested, from [0, 1] with a step of 0.1. And the result is always:
    acos(x) - 0.2f <= result <= acos(x) + 0.2f
*/
static inline float fast_acos(float x)
{
    return (-0.69813170079773212 * x * x - 0.87266462599716477) * x + 1.5707963267948966;
}

#else
static inline float fast_atan2(float y, float x)
{
    return atan2(y, x);
}
static inline float fast_acos(float x)
{
    return acos(x);
}
#endif
