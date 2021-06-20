#pragma once

#include <brutal/text/str.h>

struct source_location
{
    int line;
    str_t function;
    str_t filename;
};

#define source_location_current \
    (struct source_location) { __LINE__, str_cast(__func__), str_cast(__FILENAME__), }
