#pragma once

#ifdef _GNU_SOURCE

char *strfry(char *str);

void *memfrob(void *s, size_t n);

#endif

#if __has_include_next(<string.h>)
#    include_next <string.h>
#endif
