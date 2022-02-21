#pragma once

#include <brutal/base.h>

#if defined(__x86_64__)
#    define EMBED_HAS_MEMSET 1
#    define EMBED_HAS_MEMCPY 1
#endif

enum embed_mem_flag
{
    EMBED_MEM_NONE,
};

void embed_mem_lock(void);

void embed_mem_unlock(void);

Error embed_mem_acquire(size_t size, void **out_result, enum embed_mem_flag flags);

Error embed_mem_release(void *addr, size_t size);

void *embed_mem_set(void *d, uint8_t s, size_t c);

void *embed_mem_copy(void *to, void const *from, size_t size);
