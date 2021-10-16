#pragma once

#ifdef _BSD_SOURCE

size_t strlcpy(char *dst, char const *src, size_t size);

size_t strlcat(char *dst, char const *src, size_t size);

#endif

#if __has_include_next(<string.h>)
#    include_next <string.h>
#endif
