#pragma once

#include <brutal/base.h>

#if defined(__x86_64__)
#    define ARCH_HAS_PAUSE 1
#    define ARCH_HAS_MEMSET 1
#    define ARCH_HAS_MEMCPY 1
#endif

void arch_pause(void);

void *arch_mem_set(void *d, uint8_t s, size_t c);

void *arch_mem_copy(void *to, void const *from, size_t size);
