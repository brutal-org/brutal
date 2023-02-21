#pragma once

#include <brutal/ds/vec.h>
#include <brutal/base/str.h>

typedef struct
{
    Str path;
    Str name;
} LoaderModule;

typedef struct
{
    size_t width;
    size_t height;
} LoaderFramebuffer;

typedef struct
{
    Str name;
    Str kernel;
    Vec(LoaderModule) modules;
    LoaderFramebuffer framebuffer;
} LoaderEntry;

typedef struct
{
    Vec(LoaderEntry) entries;
} LoaderConfig;

LoaderConfig config_load(Str path, Alloc *alloc);
