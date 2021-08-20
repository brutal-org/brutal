#include <brutal/alloc.h>
#include <brutal/io.h>
#include <brutal/log.h>
#include <efi/lib.h>
#include <efi/srvs.h>
#include <efi/tty.h>
#include <elf/elf.h>
#include "loader/config.h"
#include "loader/protocol.h"

typedef void (*entry_point_func)(Handover *handover, uint64_t t) __attribute__((sysv_abi));

void __chkstk() { return; }

char *logo[] = {
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
            void *mem_segment = (void *)prog_header->virtual_address;

            mem_cpy(mem_segment, file_segment, prog_header->file_size);

            void *extra_zeroes = mem_segment + prog_header->file_size;
            uint64_t extra_zeroes_count = prog_header->memory_size - prog_header->file_size;

            if (extra_zeroes_count > 0)
            {
                mem_set(extra_zeroes, 0x00, extra_zeroes_count);
            }
        }

        prog_header = (Elf64ProgramHeader *)((void *)prog_header + elf_header->program_header_table_entry_size);
    }
}

entry_point_func loader_load_kernel(Str path)
{
    IoFile file;
    IoFileReader reader;
    Buffer buffer;

    log("Loading elf file...");
    io_file_open(&file, path);
    reader = io_file_read(&file);
    buffer = io_readall(base$(&reader), alloc_global());

    Elf64Header *header = (Elf64Header *)buffer.data;

    if (buffer.used < sizeof(Elf64Header) ||
        !elf_validate(header))
    {
        panic("Invalid elf file!");
    }

    log("Elf file loaded in memory, mapping it...");

    loader_load(header, buffer.data);

    uintptr_t entry = header->entry;

    buffer_deinit(&buffer);

    log("Entry is {#x}", entry);

    return (entry_point_func)entry;
}

void loader_boot(LoaderEntry *entry)
{
    log("Loading kernel...");

    auto entry_point = loader_load_kernel(entry->kernel);

    log("Kernel loaded, jumping in to it...");

    Handover handover = get_handover();

    entry_point(&handover, 0xC001B001);

    assert_unreachable();
}

void loader_menu(void)
{
    EFIInputKey key = efi_tty_get_key();

    while (key.scan_code != SCAN_ESC)
    {
        if (key.unicode_char == CHAR_CARRIAGE_RETURN)
        {
            efi_tty_clear();
            loader_boot(&(LoaderEntry){
                .kernel = str$("/kernel.elf"),
            });
        }

        key = efi_tty_get_key();
    }

    efi_st()->runtime_services->reset_system(EFI_RESET_SHUTDOWN, 0, 0, NULL);
}

EFIStatus efi_main(EFIHandle handle, EFISystemTable *st)
{
    efi_init(handle, st);

    st->boot_services->set_watchdog_timer(0, 0, 0, NULL);

    efi_tty_reset();
    efi_tty_clear();

    config_get_key(str_cast("test"), str_cast("/config.json"));
    
    loader_splash();
    loader_menu();

    assert_unreachable();
}
