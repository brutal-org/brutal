#pragma once

#include <brutal-text>

#include "vec.h"

#define MAP_BUCKET_COUNT (512)

typedef struct
{
    uint32_t hash;
    void *data;
    InlineStr key;
} MapNode;

typedef Vec(MapNode *) MapBucket;

typedef struct
{
    Vec(MapBucket) buckets;
    Alloc *alloc;
    int data_size;
} MapImpl;

#define Map(T)         \
    union              \
    {                  \
        MapImpl _impl; \
        T *_T;         \
    }

void map_init_impl(MapImpl *impl, int data_size, Alloc *alloc);

void map_deinit_impl(MapImpl *impl);

bool map_put_impl(MapImpl *impl, Str key, void const *data);

bool map_get_impl(MapImpl *impl, Str key, void *data);

#define map_init(SELF, ALLOC) map_init_impl(impl$(SELF), sizeof(*(SELF)->_T), ALLOC)

#define map_deinit(SELF) map_deinit_impl(impl$(SELF));

#define map_put(SELF, KEY, DATA) (                      \
    {                                                   \
        typeof(*(SELF)->_T) __value = (DATA);           \
        map_put_impl(impl$(SELF), str$(KEY), &__value); \
    })

#define map_get(SELF, KEY, DATA) (                                 \
    {                                                              \
        static_assert(sizeof(*(DATA)) == sizeof(*(SELF)->_T), ""); \
        map_get_impl(impl$(SELF), str$(KEY), (DATA));              \
    })

// clang-format off

#define map_foreach(KEY, DATA, MAP)                       \
    vec_foreach_v(__bucket, &(impl$(MAP)->buckets))       \
        vec_foreach_v(__kv, &(__bucket))                  \
            for (bool __map_once = true; __map_once;)     \
            for (Str KEY = str$(&__kv->key); __map_once;) \
            for (typeof(*(MAP)->_T) DATA = *(typeof((MAP)->_T))__kv->data; __map_once; __map_once = false)

// clang-format on
