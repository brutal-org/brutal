#pragma once

#include <brutal/base.h>
#include "arch/pmm.h"
#include "syscalls/error.h"
#include "syscalls/mem.h"

typedef void *VmmSpace;

typedef Range(size_t) VmmRange;

typedef Result(BrError, VmmRange) VmmResult;

void vmm_initialize(struct handover const *handover);

VmmSpace vmm_space_create(void);

VmmSpace vmm_kernel_space(void);

void vmm_space_destroy(VmmSpace space);

void vmm_space_switch(VmmSpace space);

VmmSpace vmm_kernel_space(void);

VmmResult vmm_alloc(
    VmmSpace space,
    PmmRange range,
    BrMemFlags flags);

VmmResult vmm_map(
    VmmSpace space,
    VmmRange virtual_range,
    PmmRange physical_range,
    BrMemFlags flags);

VmmResult vmm_unmap(VmmSpace space, VmmRange virtual_range);

PmmResult vmm_virt2phys(VmmSpace space, VmmRange virtual_range);
