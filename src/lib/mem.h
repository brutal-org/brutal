#pragma once

#include <base-std.h>

void *mem_cpy(void *s1, const void *s2, size_t n);
void *mem_set(void *str, uint8_t c, size_t n);
void *mem_move(void *dest, const void *src, size_t n);