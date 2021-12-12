#pragma once

#include <brutal/base.h>

enum embed_mem_flag
{
    EMBED_MEM_NONE,
};

void embed_mem_lock(void);

void embed_mem_unlock(void);

Error embed_mem_acquire(size_t size, void **out_result, enum embed_mem_flag flags);

Error embed_mem_release(void *addr, size_t size);
