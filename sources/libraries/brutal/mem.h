#pragma once

#include <brutal/base/std.h>
#include <brutal/mem/volatile.h>

#define MEM_PAGE_SIZE (0x1000)

void *mem_cpy(void *s1, void const *s2, size_t n);

void *mem_set(void *str, uint8_t c, size_t n);

void *mem_move(void *dest, void const *src, size_t n);
