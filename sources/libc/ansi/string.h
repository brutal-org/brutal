#pragma once

/* --- 7.24 - String handling ----------------------------------------------- */

#include <stddef.h>

/* --- 7.24.2 - Copying functions ------------------------------------------- */

void *memcpy(void *restrict s1, void const *restrict s2, size_t n);

void *memmove(void *s1, void const *s2, size_t n);

char *strcpy(char *restrict s1, char const *restrict s2);

char *strncpy(char *restrict s1, char const *restrict s2, size_t n);

/* --- 7.24.3 - Concatenation functions ------------------------------------- */

char *strcat(char *restrict s1, char const *restrict s2);

char *strncat(char *restrict s1, char const *restrict s2, size_t n);

/* --- 7.24.4 - Comparison functions ---------------------------------------- */

int memcmp(void const *s1, void const *s2, size_t n);

int strcmp(char const *s1, char const *s2);

int strcoll(char const *s1, char const *s2);

int strncmp(char const *s1, char const *s2, size_t n);

size_t strxfrm(char *restrict s1, char const *restrict s2, size_t n);

/* --- 7.24.5 - Search functions -------------------------------------------- */

void *memchr(void const *s, int c, size_t n);

char *strrchr(char const *s, int c);

size_t strcspn(char const *s1, char const *s2);

char *strpbrk(char const *s1, char const *s2);

char *strrchr(char const *s, int c);

size_t strspn(char const *s1, char const *s2);

char *strstr(char const *s1, char const *s2);

char *strtok(char *restrict s1, char const *restrict s2);

/* --- 7.24.6 - Miscellaneous functions ------------------------------------- */

void *memset(void *s, int c, size_t n);

char *strerror(int errnum);

size_t strlen(char const *s);

#if __has_include_next(<string.h>)
#    include_next <string.h>
#endif
