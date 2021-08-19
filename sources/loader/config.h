#pragma once

#include <brutal/ds.h>
#include <brutal/text.h>
#include <json/json.h>

typedef struct
{
    Str path;
} LoaderModule;

typedef struct
{
    Str kernel;
    Vec(LoaderModule) modules;
} LoaderEntry;

Str config_get_key(Str key, Str path);
