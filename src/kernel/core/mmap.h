#pragma once

#include <brutal-base/std.h>

#if defined(__x86_64)
#    define MMAP_IO_BASE (0xffff800000000000)
#    define MMAP_KERNEL_BASE (0xffffffff80000000)
#elif defined(__riscv)
#    define MMAP_IO_BASE (0)
#    define MMAP_KERNEL_BASE (0)
#else
#    error "Unsupported architecture"
#endif

static inline uintptr_t mmap_phys_to_io(uintptr_t phys_addr)
{
    return phys_addr + MMAP_IO_BASE;
}

static inline uintptr_t mmap_phys_to_kernel(uintptr_t phys_addr)
{
    return phys_addr + MMAP_KERNEL_BASE;
}

static inline uintptr_t mmap_io_to_phys(uintptr_t io_addr)
{
    return io_addr - MMAP_IO_BASE;
}

#define mmap_io_ptr_from_phys(T, ptr) ((T)(uintptr_t)mmap_phys_to_io(ptr))
