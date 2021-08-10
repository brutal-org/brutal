#ifndef __CLIB_H__
#define __CLIB_H__

#include <ctype.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

int u16strcmp(const uint16_t *l, const uint16_t *r);

int u16vsnprintf(uint16_t *buffer, size_t size, const char *fmt, va_list args);
int u16snprintf(uint16_t *buffer, size_t size, const char *fmt, ...);
size_t u16strlen(const uint16_t *str);
uint16_t *to_u16strncpy(uint16_t *dst, const char *src, size_t size);

#endif // __CLIB_H__
