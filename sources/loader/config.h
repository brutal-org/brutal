#pragma once

#include <brutal/ds.h>
#include <brutal/text.h>

typedef struct
{
    Str path;
} LoaderModule;

typedef struct
{
    Str kernel;
    Vec(LoaderModule) modules;
} LoaderEntry;
