#pragma once

#include <bruwutal/alloc/base.h>
#include <bruwutal/text/str.h>

uint16_t *utf16_str_to_cstr(Str str, Alloc *alloc);

uint16_t *utf16_str_to_cstr_dos(Str str, Alloc *alloc);
