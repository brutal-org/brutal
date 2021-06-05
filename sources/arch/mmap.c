#include "arch/mmap.h"

uintptr_t mmap_phys_to_io(uintptr_t phys)
{
    // FIXME: check for overflows
    return MMAP_IO_BASE + phys;
}
