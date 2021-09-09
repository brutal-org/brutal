#pragma once

#include <brutal/base.h>
/* --- */
#include <brutal/mem/const.h>
#include <brutal/mem/units.h>
#include <brutal/mem/volatile.h>

#define mem_is_addr_page_aligned(ADDR) ((ADDR) % MEM_PAGE_SIZE == 0)

#define mem_is_size_page_aligned(SIZE) ((SIZE) % MEM_PAGE_SIZE == 0)

#define mem_is_range_page_aligned(RANGE)       \
    (mem_is_addr_page_aligned((RANGE).base) && \
     mem_is_size_page_aligned((RANGE).size))
