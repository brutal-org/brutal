#include <brutal/debug.h>
#include <brutal/sync.h>
#include "kernel/heap.h"
#include "kernel/mmap.h"
#include "kernel/vmm.h"

VmmResult vmm_flush(MAYBE_UNUSED VmmSpace space, VmmRange virtual_range)
{
    return OK(VmmResult, virtual_range);
}

void vmm_initialize(MAYBE_UNUSED Handover const *handover)
{
}

VmmSpace vmm_space_create(void)
{
    return nullptr;
}

void vmm_space_destroy(MAYBE_UNUSED VmmSpace space)
{
}

void vmm_space_switch(MAYBE_UNUSED VmmSpace space)
{
}

VmmSpace vmm_kernel_space(void)
{
    return nullptr;
}

VmmResult vmm_map(MAYBE_UNUSED VmmSpace space, MAYBE_UNUSED VmmRange virtual_range, MAYBE_UNUSED PmmRange physical_range, MAYBE_UNUSED BrMemoryFlags flags)
{
    return OK(VmmResult, virtual_range);
}

VmmResult vmm_unmap(MAYBE_UNUSED VmmSpace space, VmmRange virtual_range)
{

    return OK(VmmResult, virtual_range);
}

PmmResult vmm_virt2phys(MAYBE_UNUSED VmmSpace space, MAYBE_UNUSED VmmRange virtual_range)
{

    return OK(PmmResult, (PmmRange){});
}
