#include <efi/base.h>
#include <efi/console.h>
#include <efi/lip.h>
#include <efi/sfsp.h>
#include <efi/st.h>
#include <efi/utils.h>
#include <stddef.h>
#include "config.h"
#include "elf.h"

EFIBootServices *BS;

void __chkstk() { return; }

char16 *logo[] = {
    u"/yyyyo:yyyys  `:osys/.",
    u"  -hMMd .yMMN oNMMNmMMMh.",
    u"   -hd   .yNyMMy.  `+MMm`",
    u"/sssss:ssssyNMM`     dMM-",
    u"  :dMMd -hMMNoMMh:` .sMMd",
    u"   :dd   -hN /mMMMMMMNs`                 Press Enter to boot",
    u".+++++y+++++y+++ydhdd+`                  Press ESC to shutdown",
    u":MMMMMMMMMMMMMMMd: dMMNs`",
    u":MMh     `MMMMd:   dMMMMd",
    u":MMh      MMMy/////mMMMMM-",
    u":MMh      MMNyMMMMMMMMMMm`",
    u":MMNmmmmmmMMN sNMMMMMMMh.",

    u".yyyyyyyyyyys  `/oyys+."};

EFIStatus efi_main(EFIHandle image_handle, EFISystemTable *system_table)
{
    /* Initializing EFI */
    init_lib(system_table, image_handle);

    clear_screen();

    system_table->console_in->reset(system_table->console_in, false);

    EFIInputKey key;
    u64 key_event = 0;

    /* Setting color to red then white */
    set_attribute(EFI_LIGHTRED);

    efi_printf("Brutal boot\r\n");

    set_attribute(EFI_WHITE);

    /* Opening config file */
    uint16_t path[] = u"efi\\config.cfg";

    File f = open_file(path);

    get_file_info(&f);

    char data_buf[20] = {0};

    get_config_key("ELF", data_buf, &f);

    TO_UTF16(utf16_ptr, data_buf);

    efi_printf("Read config file (%d bytes)\r\n\r\n", f.info.file_size, data_buf);

    close_file(&f);

    File elf_file = open_file(utf16_ptr);

    get_file_info(&elf_file);

    auto func_result = load_elf_file(utf16_ptr);
    auto result = UNWRAP_WITH_MSG(func_result, elf_get_error_message(func_result));

    efi_printf("ELF machine: %x\r\n", result.hdr.machine_type);

    for (size_t i = 0; i < sizeof(logo) / sizeof(*logo); i++)
    {

        efi_printf("%w", logo[i]);
        efi_printf("\r\n");
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
            efi_printf("Boot not available yet!\r\n");
            continue;
        }
    }

    while (1)
        ;

    return EFI_SUCCESS;
}
