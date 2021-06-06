#pragma once

typedef enum
{
    BR_MEM_NONE = 0,
    BR_MEM_WRITABLE = 1 << 1,
    BR_MEM_USER = 1 << 2,

} br_mem_flags_t;
