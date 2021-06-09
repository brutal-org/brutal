#pragma once

#include <library/text/str.h>

struct source_location
{
    int line;
    str_t function;
    str_t filename;
};

#define source_location_current \
    (struct source_location) { __LINE__, make_str(__func__), make_str(__FILENAME__), }
