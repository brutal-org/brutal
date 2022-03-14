#pragma once

#include <brutal/text.h>

typedef struct
{
    Str str;
    Alloc *alloc;
} String;

String string_make(Str str, Alloc *alloc);

void string_deinit(String *self);

void string_append(String *self, Str str);
