#pragma once
#include <brutal/text/str.h>
#include <stddef.h>
#include <stdint.h>
#include <syscalls/syscalls.h>

typedef struct
{
    BrMemObj obj;
    BrMapArgs map;
    size_t size;
    void *data;
} SharedMem;

typedef Result(BrResult, SharedMem) SharedMemResult;

SharedMem memobj_to_shared(BrMemObj obj);

BrMemObj shared_to_memobj(SharedMem from);

// shared_mem_alloc also map <!>
SharedMemResult shared_mem_alloc(size_t size);

BrResult shared_mem_free(SharedMem *self);

// use only when you receive it, shared_mem_alloc already call it
BrResult shared_mem_map(SharedMem *self);

Str shared_mem_to_str(SharedMem *self);

SharedMemResult shared_mem_from_str(Str str);
