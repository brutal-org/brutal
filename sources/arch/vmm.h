#pragma once

#include <library/base.h>

#include "arch/pmm.h"
#include "syscalls/error.h"
#include "syscalls/mem.h"

typedef void *vmm_space_t;

typedef range_t(size_t) vmm_range_t;

typedef result_t(br_error_t, vmm_range_t) vmm_result_t;

void vmm_initialize(struct handover const *handover);

vmm_space_t vmm_space_create(void);

void vmm_space_destroy(vmm_space_t space);

void vmm_space_switch(vmm_space_t space);

vmm_space_t vmm_kernel_space(void);

vmm_result_t vmm_alloc(
    vmm_space_t space,
    pmm_range_t range,
    br_mem_flags_t flags);

vmm_result_t vmm_map(
    vmm_space_t space,
    vmm_range_t virtual_range,
    pmm_range_t physical_range,
    br_mem_flags_t flags);

vmm_result_t vmm_unmap(vmm_space_t space, vmm_range_t virtual_range);

pmm_result_t vmm_virt2phys(vmm_space_t space, vmm_range_t virtual_range);
