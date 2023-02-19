#pragma once

#include <brutal-alloc>

#include "str.h"

uint16_t *utf16_str_to_cstr(Str str, Alloc *alloc);

uint16_t *utf16_str_to_cstr_dos(Str str, Alloc *alloc);
