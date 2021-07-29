#pragma once

#include <brutal/base.h>

enum host_mem_flag
{
    HOST_MEM_NONE,
};

void host_mem_lock(void);

void host_mem_unlock(void);

Error host_mem_acquire(size_t size, void **out_result, enum host_mem_flag flags);

Error host_mem_release(void *addr, size_t size);
