#pragma once

#include <stddef.h>

void *memcpy(void *restrict s1, const void *restrict s2, size_t n);
char *strrchr(const char *s, int c);
size_t strlen(const char *s);
void *memset(void *s, int c, size_t n);