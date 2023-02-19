#include <brutal-alloc>
#include <brutal-debug>
#include <brutal-io>
#include <efi/lib.h>
#include <efi/srvs.h>
#include <efi/tty.h>

#include "loader/config.h"
#include "loader/memory.h"
#include "loader/protocol.h"

Handover *allocate_handover(VmmSpace vmm)
{
    uintptr_t addr = kernel_module_phys_alloc_page((sizeof(Handover) / PAGE_SIZE) + 1);

    memory_map_range(vmm, (VmmRange){
                              .base = addr + MMAP_KERNEL_BASE,
                              .size = align_up$(sizeof(Handover), PAGE_SIZE),
                          },
                     (PmmRange){
                         .base = addr,
                         .size = align_up$(sizeof(Handover), PAGE_SIZE),
                     });

    return (Handover *)addr;
}

void loader_boot(LoaderEntry const *entry)
{
    VmmSpace vmm = memory_create();

    log$("Loading kernel...");

    log$("Kernel loaded, jumping in to it...");
    Handover *handover = allocate_handover(vmm);
    EntryPointFn *entry_point = efi_populate_handover(entry, handover, vmm);

    log$("Jumping in...");
    efi_deinit();
    memory_switch(vmm);
    entry_point(((void *)handover) + MMAP_KERNEL_BASE, 0xC001B001);

    panic$("entry_point should no return!");
}

EfiStatus efi_main(EFIHandle handle, EFISystemTable *st)
{
    efi_init(handle, st);
    st->boot_services->set_watchdog_timer(0, 0, 0, nullptr);

    efi_tty_reset();

    LoaderConfig config = config_load(str$("/boot/config.json"), alloc_global());

    assert_truth(vec_len(&config.entries) > 0);
    loader_boot(&vec_at(&config.entries, 0));

    panic$("loader_menu should no return!");
}
