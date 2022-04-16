#pragma once

#include <brutal-base>

void *mem_cpy(void *s1, void const *s2, size_t n);

void *mem_set(void *str, uint8_t c, size_t n);

void *mem_move(void *dest, void const *src, size_t n);
