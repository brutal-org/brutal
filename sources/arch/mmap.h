#pragma once

#include <library/base/std.h>

#define MMAP_IO_BASE 0xffff800000000000
#define MMAP_KERNEL_BASE 0xffffffff80000000

uintptr_t mmap_phys_to_io(uintptr_t phys);
