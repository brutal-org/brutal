#pragma once

#include "keywords.h"
#include "macros.h"
#include "std.h"

// x + 0 is for removing the const qualifier

#define bswap$(VALUE) (                      \
    {                                        \
        typeof(VALUE + 0) _v = (VALUE);      \
        typeof(VALUE + 0) _result = 0;       \
        if (sizeof(VALUE) == 8)              \
            _result = __builtin_bswap64(_v); \
        if (sizeof(VALUE) == 4)              \
            _result = __builtin_bswap32(_v); \
        if (sizeof(VALUE) == 2)              \
            _result = __builtin_bswap16(_v); \
        if (sizeof(VALUE) == 1)              \
            _result = _v;                    \
        _result;                             \
    })

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#    define swap_little_endian$(VALUE) (VALUE)
#else
#    define swap_little_endian$(VALUE) bswap$(VALUE)
#endif

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#    define swap_big_endian$(VALUE) bswap$(VALUE)
#else
#    define swap_big_endian$(VALUE) (VALUE)
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

typedef BigEndian(int8_t) be_int8_t;
typedef BigEndian(int16_t) be_int16_t;
typedef BigEndian(int32_t) be_int32_t;
typedef BigEndian(int64_t) be_int64_t;

typedef LittleEndian(uint8_t) le_uint8_t;
typedef LittleEndian(uint16_t) le_uint16_t;
typedef LittleEndian(uint32_t) le_uint32_t;
typedef LittleEndian(uint64_t) le_uint64_t;

typedef LittleEndian(int8_t) le_int8_t;
typedef LittleEndian(int16_t) le_int16_t;
typedef LittleEndian(int32_t) le_int32_t;
typedef LittleEndian(int64_t) le_int64_t;

static_assert(sizeof(be_uint8_t) == sizeof(uint8_t), "be and native are expected to be of the same size");
static_assert(sizeof(be_uint16_t) == sizeof(uint16_t), "be and native are expected to be of the same size");
static_assert(sizeof(be_uint32_t) == sizeof(uint32_t), "be and native are expected to be of the same size");
static_assert(sizeof(be_uint64_t) == sizeof(uint64_t), "be and native are expected to be of the same size");

static_assert(sizeof(be_int8_t) == sizeof(int8_t), "be and native are expected to be of the same size");
static_assert(sizeof(be_int16_t) == sizeof(int16_t), "be and native are expected to be of the same size");
static_assert(sizeof(be_int32_t) == sizeof(int32_t), "be and native are expected to be of the same size");
static_assert(sizeof(be_int64_t) == sizeof(int64_t), "be and native are expected to be of the same size");

static_assert(sizeof(le_int8_t) == sizeof(int8_t), "le and native are expected to be of the same size");
static_assert(sizeof(le_int16_t) == sizeof(int16_t), "le and native are expected to be of the same size");
static_assert(sizeof(le_int32_t) == sizeof(int32_t), "le and native are expected to be of the same size");
static_assert(sizeof(le_int64_t) == sizeof(int64_t), "le and native are expected to be of the same size");

#define be_store$(VALUE, DEST) ({ (DEST)->_be_data = swap_big_endian$((typeof((DEST)->_be_data))VALUE); })
#define be_load$(VALUE) swap_big_endian$((VALUE)._be_data)
#define be$(T, VALUE) \
    ((T){swap_big_endian$(VALUE)})

#define le_store$(VALUE, DEST) ({ (DEST)->_le_data = swap_little_endian$((typeof((DEST)->_le_data))VALUE); })
#define le_load$(VALUE) swap_little_endian$((VALUE)._le_data)
#define le$(T, VALUE) \
    ((T){swap_little_endian$(VALUE)})
