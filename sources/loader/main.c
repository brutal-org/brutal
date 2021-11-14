#include <brutal/alloc.h>
#include <brutal/debug.h>
#include <brutal/io.h>
#include <efi/lib.h>
#include <efi/srvs.h>
#include <efi/tty.h>
#include <elf/elf.h>
#include "loader/config.h"
#include "loader/memory.h"
#include "loader/protocol.h"

typedef void (*EntryPointFn)(Handover *handover, uint64_t t) __attribute__((sysv_abi));

void __chkstk() { return; }

static char const *logo[] = {
    "/yyyyo:yyyys  `:osys/.",
    "  -hMMd .yMMN oNMMNmMMMh.",
    "   -hd   .yNyMMy.  `+MMm`",
    "/sssss:ssssyNMM`     dMM-",
    "  :dMMd -hMMNoMMh:` .sMMd",
    "   :dd   -hN /mMMMMMMNs`                 Press Enter to boot",
    ".+++++y+++++y+++ydhdd+`                  Press ESC to shutdown",
    ":MMMMMMMMMMMMMMMd: dMMNs`",
    ":MMh     `MMMMd:   dMMMMd",
    ":MMh      MMMy/////mMMMMM-",
    ":MMh      MMNyMMMMMMMMMMm`",
    ":MMNmmmmmmMMN sNMMMMMMMh.",
    ".yyyyyyyyyyys  `/oyys+.",
};

void loader_splash(void)
{
    efi_tty_set_attribute(EFI_BLUE);

    print(io_std_out(), "Brutal boot\n");

    efi_tty_set_attribute(EFI_WHITE);

    for (size_t i = 0; i < sizeof(logo) / sizeof(*logo); i++)
    {
        print(io_std_out(), "{}\n", logo[i]);
    }
}

void loader_load(Elf64Header const *elf_header, void *base)
{
    Elf64ProgramHeader *prog_header = (Elf64ProgramHeader *)(base + elf_header->program_header_table_file_offset);

    for (int i = 0; i < elf_header->program_header_table_entry_count; i++)
    {
        if (prog_header->type == ELF_PROGRAM_HEADER_LOAD)
        {
            void *file_segment = (void *)((uint64_t)base + prog_header->file_offset);

            void *mem_phys_segment = (void *)kernel_module_phys_alloc_page_addr(ALIGN_UP(prog_header->memory_size, PAGE_SIZE) / PAGE_SIZE, prog_header->virtual_address - 0xffffffff80000000);

            mem_cpy(mem_phys_segment, file_segment, prog_header->file_size);
            mem_set(mem_phys_segment + prog_header->file_size, 0, prog_header->memory_size - prog_header->file_size);
            memory_map_range((VmmRange){
                                 .base = prog_header->virtual_address,
                                 .size = ALIGN_UP(prog_header->memory_size, PAGE_SIZE),
                             },
                             (PmmRange){
                                 .base = (uintptr_t)mem_phys_segment,
                                 .size = ALIGN_UP(prog_header->memory_size, PAGE_SIZE),
                             });
        }
        else 
        {
            log$("unkown header: {}", prog_header->type);
        }

        prog_header = (Elf64ProgramHeader *)((void *)prog_header + elf_header->program_header_table_entry_size);
    }
}

EntryPointFn loader_load_kernel(Str path)
{
    IoFile file;
    IoReader reader;
    Buf buf;

    log$("Loading elf file...");
    io_file_open(&file, path);
    reader = io_file_reader(&file);
    buf = io_readall((&reader), alloc_global());

    log$("Loaded elf file...");
    Elf64Header *header = (Elf64Header *)buf.data;

    if (buf.used < sizeof(Elf64Header) ||
        !elf_validate(header))
    {
        panic$("Invalid elf file!");
    }

    log$("Elf file loaded in memory, mapping it...");

    loader_load(header, buf.data);

    uintptr_t entry = header->entry;

    buf_deinit(&buf);

    log$("Entry is {#x}", entry);

    return (EntryPointFn)entry;
}
#define MMAP_KERNEL_BASE (0xffffffff80000000)
Handover* allocate_handover(void)
{
    uintptr_t handover_copy_phys = kernel_module_phys_alloc_page((sizeof(Handover)/ PAGE_SIZE)+1);

    memory_map_range((VmmRange){
        .base = handover_copy_phys + MMAP_KERNEL_BASE,
        .size = ALIGN_UP(sizeof(Handover), PAGE_SIZE),
    }, 
    (PmmRange){
        .base = handover_copy_phys,
        .size = ALIGN_UP(sizeof(Handover), PAGE_SIZE),
    });
    memory_flush_tlb();
    return (Handover*)(handover_copy_phys + MMAP_KERNEL_BASE);
}

void loader_boot(LoaderEntry *entry)
{
    log$("Loading kernel...");

    EntryPointFn entry_point = loader_load_kernel(entry->kernel);

    log$("Kernel loaded, jumping in to it...");
    Handover* handover = allocate_handover();
    *handover = get_handover();

    efi_deinit();

    entry_point(handover, 0xC001B001);

    panic$("entry_point should no return!");
}

void loader_menu(void)
{
    EFIInputKey key = efi_tty_get_key();
    LoaderEntry entry = config_get_entry(str$("Brutal"), str$("/config.json"));

    while (key.scan_code != SCAN_ESC)
    {
        if (key.unicode_char == CHAR_CARRIAGE_RETURN)
        {
            efi_tty_clear();
            loader_boot(&entry);
        }

        key = efi_tty_get_key();
    }

    efi_st()->runtime_services->reset_system(EFI_RESET_SHUTDOWN, 0, 0, nullptr);
}

EFIStatus efi_main(EFIHandle handle, EFISystemTable *st)
{
    efi_init(handle, st);

    st->boot_services->set_watchdog_timer(0, 0, 0, nullptr);

    efi_tty_reset();
    efi_tty_clear();

    memory_init();
    loader_splash();
    loader_menu();

    panic$("loader_menu should no return!");
}
