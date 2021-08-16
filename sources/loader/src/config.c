#include <brutal/base.h>
#include <brutal/host.h>
#include <brutal/io.h>
#include <efi/utils.h>
#include "brutal/host/log.h"
#include "brutal/host/mem.h"
#include "brutal/io/scan.h"
#include "brutal/mem.h"
#include "config.h"

static Str get_until_c(Scan *scan, bool (*func)(int), Alloc *alloc)
{
    char *buf = alloc_malloc(alloc, 128);

    int size = 0;
    int buf_size = 128;

    buf[size++] = scan_curr(scan);

    while (func(scan_next(scan)) != true && !scan_end(scan))
    {
        if (func(scan_curr(scan)) == false)
        {
            buf[size++] = scan_curr(scan);

            if (size >= buf_size)
            {
                buf_size *= 2;
                buf = alloc_resize(alloc, buf, buf_size);
            }
        }
    }

    buf[size] = '\0';

    return str_cast(buf);
}

static bool is_brace(int c)
{
    return c == '{' || c == '}';
}

static bool is_equal_sign(int c)
{
    return c == '=';
}

static bool is_semicolon(int c)
{
    return c == ';';
}

static char *removechar(char str[], char t)
{
    size_t i = 0, j;

    while (i < cstr_len(str))
    {
        if (str[i] == t)
        {
            for (j = i; j < cstr_len(str); j++)
            {
                str[j] = str[j + 1];
            }
        }

        else
        {
            i++;
        }
    }

    return str;
}

static char *remove_useless_chars(char str[])
{
    return removechar(removechar(removechar(removechar(str, '\r'), '\t'), '\n'), ' ');
}

static ConfigEntry parse_config(File *file)
{
    ConfigEntry ret;

    char text[file->info.file_size];

    read_file(file, text);

    text[file->info.file_size] = '\0';

    Scan scanner;

    scan_init(&scanner, str_cast(text));

    char entry_buf[50] = {0};
    int size = 0;

    bool is_entry = false;

    while (scan_next(&scanner) != ' ')
    {
        if (scan_curr(&scanner) == ' ')
        {
            break;
        }

        entry_buf[size++] = scan_curr(&scanner);
    }

    if (str_eq(str_cast(entry_buf), str_cast("entry")))
    {
        is_entry = true;
        mem_set(entry_buf, 0, size);
    }

    Str raw_string = get_until_c(&scanner, is_brace, alloc_global());
    int start_pos = scanner.head;

    ret.name = str_cast(remove_useless_chars(removechar(raw_string.buffer, '"')));

    scanner.head = start_pos;

    while (scan_next(&scanner) != '}')
    {

        Str key_name = str_cast(remove_useless_chars(removechar(get_until_c(&scanner, is_equal_sign, alloc_global()).buffer, ' ')));

        Str key_data = str_cast(remove_useless_chars(removechar(get_until_c(&scanner, is_semicolon, alloc_global()).buffer, '"')));

        ret.keys[ret.num_of_keys].data = key_data;
        ret.keys[ret.num_of_keys].name = key_name;

        ret.num_of_keys++;

        if (scan_peek(&scanner, 1) == '}')
        {
            return ret;
        }
    }

    return ret;
}

char *get_config_key(char *key, char *data, File *file)
{
    ConfigEntry entry = parse_config(file);

    ConfigKey curr_key = entry.keys[0];

    bool is_found = false;

    for (int i = 0; i < entry.num_of_keys; i++)
    {
        curr_key = entry.keys[i];

        if (str_eq(curr_key.name, str_cast(key)) == true)
        {
            is_found = true;
            break;
        }
    }

    if (!is_found)
    {
        panic("Couldn't find key {}", key);
    }
    
    mem_cpy(data, curr_key.data.buffer, curr_key.data.len);

    return data;
}
