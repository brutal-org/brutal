#pragma once

#ifdef _BSD_SOURCE

size_t strlcpy(char *dst, char const *src, size_t size);

size_t strlcat(char *dst, char const *src, size_t size);

#endif

#ifdef _GNU_SOURCE

char *strfry(char *str);

void *memfrob(void *s, size_t n);

#endif

#ifdef _BRUTAL_SOURCE

void memswap(void *block1, void *block2, size_t n);

#endif

#if __has_include_next(<string.h>)
#    include_next <string.h>
#endif
