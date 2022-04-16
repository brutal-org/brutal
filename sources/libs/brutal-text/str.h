#pragma once

#include <brutal-alloc>

Str str_dup(Str const str, Alloc *alloc);

Str str_concat(Str const lhs, Str const rhs, Alloc *alloc);

bool str_eq_ci(Str const lhs, Str const rhs);

int str_count(Str const lStr, Str const rStr);

int str_count_chr(Str const str, uint8_t chr);

int str_last(Str const lStr, Str const rStr);

int str_last_chr(Str const str, uint8_t chr);

int str_first(Str const lStr, Str const rStr);

int str_first_chr(Str const str, uint8_t chr);


