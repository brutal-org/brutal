#include <brutal-hash>

#include "map.h"

static uint32_t map_hash(Str key)
{
    return fnv_32(key.buf, key.len, FNV1_32_INIT);
}

static MapNode *map_node_create(Alloc *alloc, Str key, void const *data, int data_size)
{
    int node_size = sizeof(MapNode) + align_up$(key.len, 16) + data_size;
    MapNode *node = (MapNode *)alloc_malloc(alloc, node_size);

    node->hash = map_hash(key);

    node->data = (uint8_t *)node + sizeof(MapNode) + align_up$(key.len, 16);

    mem_cpy(&node->key.buf, key.buf, key.len);
    node->key.len = key.len;

    mem_cpy(node->data, data, data_size);

    return node;
}

static void map_node_destroy(Alloc *alloc, MapNode *node)
{
    alloc_free(alloc, node);
}

static MapBucket *map_bucket(MapImpl *impl, uint32_t hash)
{
    return &impl->buckets.data[hash % MAP_BUCKET_COUNT];
}

static MapNode *map_node(MapImpl *impl, Str key, uint32_t hash)
{
    MapBucket *bucket = map_bucket(impl, hash);

    vec_foreach_v(node, bucket)
    {
        if (node->hash == hash &&
            str_eq(key, str$(&node->key)))
        {
            return node;
        }
    }

    return nullptr;
}

void map_init_impl(MapImpl *impl, int data_size, Alloc *alloc)
{
    *impl = (MapImpl){};

    vec_init(&impl->buckets, alloc);

    for (size_t i = 0; i < MAP_BUCKET_COUNT; i++)
    {
        MapBucket bucket;
        vec_init(&bucket, alloc);
        vec_push(&impl->buckets, bucket);
    }

    impl->alloc = alloc;
    impl->data_size = data_size;
}

void map_deinit_impl(MapImpl *impl)
{
    vec_foreach_v(bucket, &impl->buckets)
    {
        vec_foreach_v(node, &bucket)
        {
            map_node_destroy(impl->alloc, node);
        }

        vec_deinit(&bucket);
    }

    vec_deinit(&impl->buckets);
}

bool map_put_impl(MapImpl *impl, Str key, void const *data)
{
    uint32_t hash = map_hash(key);
    MapNode *node = map_node(impl, key, hash);

    if (node)
    {
        mem_cpy(node->data, data, impl->data_size);
    }
    else
    {
        MapBucket *bucket = map_bucket(impl, hash);
        node = map_node_create(impl->alloc, key, data, impl->data_size);
        vec_push(bucket, node);
    }

    return true;
}

bool map_get_impl(MapImpl *impl, Str key, void *data)
{
    uint32_t hash = map_hash(key);
    MapNode *node = map_node(impl, key, hash);

    if (node)
    {
        mem_cpy(data, node->data, impl->data_size);
        return true;
    }

    return node != nullptr;
}
