#pragma once
#include <brutal/text/str.h>
#include <stddef.h>
#include <stdint.h>
#include <syscalls/syscalls.h>

typedef struct
{
    size_t len;
    uint8_t data[];
} SharedMemData;

typedef struct
{
    BrMemObj obj;
    BrMapArgs map;
    SharedMemData *data;
} SharedMem;

SharedMem memobj_to_shared(BrMemObj obj);

BrMemObj shared_to_memobj(SharedMem from);

// shared_mem_alloc also map <!>
SharedMem shared_mem_alloc(size_t size);

void shared_mem_free(SharedMem *self);

// use only when you receive it, shared_mem_alloc already call it
void *shared_mem_map(SharedMem *self);

void shared_mem_unmap(SharedMem *self);

#define shared_mem_to_str(MEM) \
    (Str) { (MEM)->data->len, (char *)(MEM)->data->data }
