#pragma once

#include <brutal/base/keywords.h>
#include <brutal/base/macros.h>
#include <brutal/base/std.h>

#define bswap(VALUE) (                       \
    {                                        \
        auto _v = (VALUE);                   \
        typeof(VALUE) _result = 0;           \
        if (sizeof(_v) == 8)                 \
            _result = __builtin_bswap64(_v); \
        if (sizeof(_v) == 4)                 \
            _result = __builtin_bswap32(_v); \
        if (sizeof(_v) == 2)                 \
            _result = __builtin_bswap16(_v); \
        if (sizeof(_v) == 1)                 \
            _result = _v;                    \
        _result;                             \
    })

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#    define swap_little_endian(VALUE) (VALUE)
#else
#    define swap_little_endian(VALUE) bswap(VALUE)
#endif

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#    define swap_big_endian(VALUE) bswap(VALUE)
#else
#    define swap_big_endian(VALUE) (VALUE)
#endif

#define BigEndian(T) \
    struct           \
    {                \
        T _be_data;  \
    }

#define LittleEndian(T) \
    struct              \
    {                   \
        T _le_data;     \
    }

typedef BigEndian(uint8_t) be_uint8_t;
typedef BigEndian(uint16_t) be_uint16_t;
typedef BigEndian(uint32_t) be_uint32_t;
typedef BigEndian(uint64_t) be_uint64_t;

typedef LittleEndian(uint8_t) le_uint8_t;
typedef LittleEndian(uint16_t) le_uint16_t;
typedef LittleEndian(uint32_t) le_uint32_t;
typedef LittleEndian(uint64_t) le_uint64_t;

static_assert(sizeof(be_uint8_t) == sizeof(uint8_t), "be and native are expected to be of the same size");
static_assert(sizeof(be_uint16_t) == sizeof(uint16_t), "be and native are expected to be of the same size");
static_assert(sizeof(be_uint32_t) == sizeof(uint32_t), "be and native are expected to be of the same size");
static_assert(sizeof(be_uint64_t) == sizeof(uint64_t), "be and native are expected to be of the same size");

static_assert(sizeof(le_uint8_t) == sizeof(uint8_t), "le and native are expected to be of the same size");
static_assert(sizeof(le_uint16_t) == sizeof(uint16_t), "le and native are expected to be of the same size");
static_assert(sizeof(le_uint32_t) == sizeof(uint32_t), "le and native are expected to be of the same size");
static_assert(sizeof(le_uint64_t) == sizeof(uint64_t), "le and native are expected to be of the same size");

#define store_be(VALUE, DEST) ({ (DEST)->_be_data = swap_big_endian(VALUE); })
#define load_be(VALUE) swap_big_endian((VALUE)._be_data)

#define store_le(VALUE, DEST) ({ (DEST)->_le_data = swap_little_endian(VALUE); })
#define load_le(VALUE) swap_little_endian((VALUE)._le_data)
