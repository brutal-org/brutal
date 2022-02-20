#pragma once
#include <bal/boot.h>

// Entry of the loader, called by the platform's entry point. This is needed because of EFI requiring parameters to be passed to its entry point
void loader_entry(void);
void loader_boot_deinit(void);

// Handover functions, used by the loader to fill the boot protocol properly.
void loader_handover_fill_mmap(HandoverMmap *self);

void loader_handover_fill_framebuffer(HandoverFramebuffer *fb, uint64_t requested_width, uint64_t requested_height);

uintptr_t loader_handover_get_rsdp();

// Memory allocation
enum loader_mem_flag
{
    LOADER_MEM_DATA_PAGES,
    LOADER_MEM_KERNEL_PAGES,
    LOADER_MEM_ADDR,
};

Error loader_acquire_pages(size_t count, uintptr_t *ret, enum loader_mem_flag flags);
