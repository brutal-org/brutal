#include <brutal/host/log.h>
#include <efi/base.h>
#include <efi/console.h>
#include <efi/lip.h>
#include <efi/sfsp.h>
#include <efi/st.h>
#include <efi/utils.h>
#include <handover/handover.h>
#include <stddef.h>
#include "config.h"
#include "efi/bs.h"
#include "efi/dpp.h"
#include "elf.h"
#include "loader/src/protocol.h"

EFIBootServices *BS;

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
    ".yyyyyyyyyyys  `/oyys+."};

EFIStatus efi_main(EFIHandle image_handle, EFISystemTable *system_table)
{
    /* Initializing EFI */
    init_lib(system_table, image_handle);

    clear_screen();

    system_table->boot_services->set_watchdog_timer(0, 0, 0, NULL);
    system_table->console_in->reset(system_table->console_in, false);

    EFIInputKey key;
    u64 key_event = 0;

    /* Setting color to red then white */
    set_attribute(EFI_LIGHTRED);

    efi_print("Brutal boot");

    set_attribute(EFI_WHITE);

    /* Opening config file */
    uint16_t path[] = u"efi\\config.cfg";

    File f = open_file(path);

    get_file_info(&f);

    char data_buf[40] = {0};

    get_config_key("kernel", data_buf, &f);

    u16 *utf16_ptr = to_utf16(data_buf);

    efi_print("Read config file ({} bytes)\r\n", f.info.file_size);
    
    close_file(&f);

    File elf_file = open_file(utf16_ptr);

    get_file_info(&elf_file);

    auto func_result = load_elf_file(utf16_ptr);

    auto result = UNWRAP_OR_MESSAGE(func_result, elf_get_error_message(func_result));

    for (size_t i = 0; i < sizeof(logo) / sizeof(*logo); i++)
    {

        efi_print("{}", logo[i]);
    }

    while ((u64)key.scan_code != SCAN_ESC)
    {
        system_table->boot_services->wait_for_event(1, &system_table->console_in->wait_for_key, &key_event);
        system_table->console_in->read_key(system_table->console_in, &key);
        system_table->console_in->reset(system_table->console_in, false);

        if ((u64)key.scan_code == SCAN_ESC)
        {
            system_table->runtime_services->reset_system(EFI_RESET_SHUTDOWN, 0, 0, NULL);
        }

        else if ((u64)key.unicode_char == CHAR_CARRIAGE_RETURN)
        {
            goto boot;
            break;
        }
    }

boot:
{
    void (*entry)(Handover *);

    entry = (void (*)(Handover *))result.entry_point;

    clear_screen();

    auto handover = get_handover(system_table, image_handle);

    // passing data to the kernel in a hacky way lol
    asm volatile("mov %0, %%rdi" ::"a"(&handover));

    entry(&handover);
}

    while (1)
        ;
    return EFI_SUCCESS;
}
