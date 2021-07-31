#pragma once

#include <brutal/base/std.h>
#include <brutal/mem/units.h>
#include <brutal/mem/volatile.h>

#define MEM_PAGE_SIZE (0x1000)

#define mem_is_addr_page_aligned(ADDR) ((ADDR) % MEM_PAGE_SIZE == 0)

#define mem_is_size_page_aligned(SIZE) ((SIZE) % MEM_PAGE_SIZE == 0)

#define mem_is_range_page_aligned(RANGE)       \
    (mem_is_addr_page_aligned((RANGE).base) && \
     mem_is_size_page_aligned((RANGE).size))

void *mem_cpy(void *s1, void const *s2, size_t n);

void *mem_set(void *str, uint8_t c, size_t n);

void *mem_move(void *dest, void const *src, size_t n);
