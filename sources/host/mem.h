#pragma once

#include <library/base/error.h>
#include <library/base/std.h>

#define HOST_MEM_PAGESIZE 4096

enum host_mem_flag
{
    HOST_MEM_NONE,
};

void host_mem_lock(void);

void host_mem_unlock(void);

error_t host_mem_acquire(size_t size, void **out_result, enum host_mem_flag flags);

error_t host_mem_commit(void *addr, size_t size);

error_t host_mem_decommit(void *addr, size_t size);

error_t host_mem_release(void *addr, size_t size);
