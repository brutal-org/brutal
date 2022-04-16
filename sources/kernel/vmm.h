#pragma once

#include <bal/abi.h>
#include "kernel/pmm.h"

typedef void *VmmSpace;

typedef USizeRange VmmRange;

typedef Result(BrResult, VmmRange) VmmResult;

void vmm_initialize(Handover const *handover);

VmmSpace vmm_space_create(void);

VmmSpace vmm_kernel_space(void);

void vmm_space_destroy(VmmSpace space);

void vmm_space_switch(VmmSpace space);

VmmSpace vmm_kernel_space(void);

VmmResult vmm_map(VmmSpace space, VmmRange virtual_range, PmmRange physical_range, BrMemoryFlags flags);

VmmResult vmm_update_flags(VmmSpace space, VmmRange virtual_range, BrMemoryFlags flags);

VmmResult vmm_unmap(VmmSpace space, VmmRange virtual_range);

PmmResult vmm_virt2phys(VmmSpace space, VmmRange virtual_range);

VmmResult vmm_flush(VmmSpace space, VmmRange virtual_range);
