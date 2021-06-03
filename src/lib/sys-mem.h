#pragma once

#include <base-std.h>

#define SYS_MEM_PAGESIZE 4096

enum sys_mem_result
{
    SYS_MEM_SUCCESS,
};

enum sys_mem_result sys_mem_acquire(size_t size, void **out_result);

enum sys_mem_result sys_mem_commit(void *addr, size_t size);

enum sys_mem_result sys_mem_decommit(void *addr, size_t size);

enum sys_mem_result sys_mem_release(void *addr, size_t size);
