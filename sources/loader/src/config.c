#include <ansi/string.h>
#include <brutal/base.h>
#include <efi/utils.h>
#include "brutal/mem.h"
#include "config.h"

char *strncpy(char *dst, const char *src, size_t size)
{
    size_t i = 0;

    for (; i < size && src[i]; ++i)
        dst[i] = src[i];

    for (; i < size; ++i)
        dst[i] = '\0';

    return dst;
}

char *get_config_key(char *key, char *data, File *file)
{
    char *text = efi_malloc(file->info.file_size);

    read_file(file, text);

    char __key[strlen(key)];

    char c, kv[100] = {0};

    int i = 0;

    while ((c = *text++))
    {
        /* ignoring spaces and tabs */
        if (c == '\t' || c == ' ' || c == '\b')
            continue;

        else if (c == '=')
        {
            /* finished reading a key */
            kv[i] = 0x0;

            strncpy(__key, kv, strlen(kv));

            memset(kv, 0, sizeof(kv));

            i = 0;
            continue;
        }

        else if (c == '\n')
        {
            /* finished reading a value */
            kv[i] = 0x0;

            if (strcmp(__key, key) == 0)
            {
                strncpy(data, kv, strlen(kv));

                memset(__key, 0, sizeof(__key));

                break;
            }

            memset(kv, 0, sizeof(kv));

            i = 0;
            continue;
        }

        kv[i++] = c;
    }

    efi_free(text);

    return data;
}
