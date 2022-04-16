#pragma once

#include <brutal-base>

/* Based on https://github.com/haipome/fnv */

/* --- FNV Hash (32bit) ----------------------------------------------------- */

typedef uint32_t Fnv32;

#define FNV1_32_INIT ((Fnv32)0x811c9dc5)
#define FNV_32_PRIME ((Fnv32)0x01000193)

Fnv32 fnv_32(void const *buf, size_t len, Fnv32 hashval);

Fnv32 fnv_32a(void const *buf, size_t len, Fnv32 hashval);

/* --- FNV Hash (64bit) ----------------------------------------------------- */

typedef uint64_t Fnv64;

#define FNV1_64_INIT ((Fnv64)0xcbf29ce484222325ULL)
#define FNV_64_PRIME ((Fnv64)0x100000001b3ULL)

Fnv64 fnv_64(void const *buf, size_t len, Fnv64 hashval);

Fnv64 fnv_64a(void const *buf, size_t len, Fnv64 hashval);
