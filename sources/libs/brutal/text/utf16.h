#pragma once

#include <brutal/alloc/base.h>
#include <brutal/text/str.h>

uint16_t *str_to_cstr_utf16(Str str, Alloc *alloc);

uint16_t *str_to_cstr_utf16_dos(Str str, Alloc *alloc);
