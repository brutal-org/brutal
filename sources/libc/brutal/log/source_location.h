#pragma once

#include <brutal/text/str.h>

typedef struct
{
    int line;
    Str function;
    Str filename;
} SourceLocation;

#define source_location_current \
    (SourceLocation) { __LINE__, str_cast(__func__), str_cast(__FILENAME__), }
