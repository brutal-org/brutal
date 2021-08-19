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

LoaderEntry config_get_entry(Str name, Str path, Buffer* buffer);
