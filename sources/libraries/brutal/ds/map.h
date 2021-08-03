#pragma once

#include <brutal/ds/vec.h>
#include <brutal/text/str.h>

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

bool map_put_impl(MapImpl *impl, Str key, const void *data);

bool map_get_impl(MapImpl *impl, Str key, void *data);

#define map_init(SELF, ALLOC) map_init_impl(impl_cast(SELF), sizeof(*(SELF)->_T), ALLOC)

#define map_deinit(SELF) map_deinit_impl(impl_cast(SELF));

#define map_put(SELF, KEY, DATA) (                            \
    {                                                         \
        typeof(*(SELF)->_T) value = (DATA);                   \
        map_put_impl(impl_cast(SELF), str_cast(KEY), &value); \
    })

#define map_get(SELF, KEY, DATA) map_get_impl(impl_cast(SELF), str_cast(KEY), (DATA))

// clang-format off

#define map_foreach(KEY, DATA, MAP)                          \
    vec_foreach(__bucket, &((MAP)->buckets))                 \
        vec_foreach(__kv, &(__b))                            \
            for (bool __map_once = true; __map_once;)        \
            for (Str KEY = str_cast(&_kv->key); __map_once;) \
            for (typeof(*(MAP)->_T) DATA = *(typeof(impl_cast(MAP)->_T))__kv->data; __map_once; __map_once = false)

// clang-format on
