#pragma once

#include <brutal/base.h>
#include "arch/pmm.h"
#include "syscalls/error.h"
#include "syscalls/mem.h"

typedef void *vmm_space_t;

typedef Range(size_t) VmmRange;

typedef Result(BrError, VmmRange) vmm_Result;

void vmm_initialize(struct handover const *handover);

vmm_space_t vmm_space_create(void);

vmm_space_t vmm_kernel_space(void);

void vmm_space_destroy(vmm_space_t space);

void vmm_space_switch(vmm_space_t space);

vmm_space_t vmm_kernel_space(void);

vmm_Result vmm_alloc(
    vmm_space_t space,
    PmmRange range,
    BrMemFlags flags);

vmm_Result vmm_map(
    vmm_space_t space,
    VmmRange virtual_range,
    PmmRange physical_range,
    BrMemFlags flags);

vmm_Result vmm_unmap(vmm_space_t space, VmmRange virtual_range);

pmm_Result vmm_virt2phys(vmm_space_t space, VmmRange virtual_range);
