#include <brutal/base.h>
#include <efi/utils.h>
#include "brutal/mem.h"
#include "config.h"

char *get_config_key(char *key, char *data, File *file)
{
    char *text = alloc_malloc(alloc_global(), file->info.file_size);

    read_file(file, text);

    text[file->info.file_size] = '\0';

    char __key[cstr_len(key)];

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
            kv[i] = '\0';

            auto kv_str = str_cast(kv);

            mem_cpy(__key, kv, kv_str.len);

            __key[kv_str.len] = '\0';

            mem_set(kv, 0, sizeof(kv));

            i = 0;
            continue;
        }

        else if (c == '\n')
        {
            /* finished reading a value */
            kv[i] = '\0';

            if (str_eq(str_cast(__key), str_cast(key)))
            {
                auto kv_str = str_cast(kv);

                mem_cpy(data, kv, kv_str.len);

                data[kv_str.len] = '\0';
		
                mem_set(__key, 0, sizeof(__key));

                break;
            }

            mem_set(kv, 0, sizeof(kv));

            i = 0;
            continue;
        }

        kv[i++] = c;
    }

    host_mem_release(text, 0);

    return data;
}
