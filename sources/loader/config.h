#pragma once

#include <brutal/ds.h>
#include <brutal/text.h>

typedef struct
{
    Str path;
    Str name;
} LoaderModule;

typedef struct
{
    Str name;
    Str kernel;
    Vec(LoaderModule) modules;
} LoaderEntry;

LoaderEntry config_get_entry(Str name, Str path);
