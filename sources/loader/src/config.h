#pragma once
#include <efi/utils.h>

typedef struct
{
    Str name;
    Str data;
} ConfigKey;
typedef struct
{
    Str name;
    Str modules[16];
    ConfigKey keys[16];
    int num_of_keys;
} ConfigEntry;

char *get_config_key(char *key, char *data, File *file);
