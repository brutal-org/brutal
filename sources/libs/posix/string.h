#pragma once

#include <locale/local_t.h>

void *memccpy(void *restrict s1, void const *restrict s2, int c, size_t n);

char *stpcpy(char *restrict s1, char const *restrict s2);

char *stpncpy(char *restrict s1, char const *restrict s2, size_t n);

int strcoll_l(char const *, char const *, locale_t);

char *strdup(char const *);

char *strerror_l(int, locale_t);

int strerror_r(int, char *, size_t);

char *strndup(char const *, size_t);

size_t strnlen(char const *, size_t);

char *strsignal(int);

char *strtok_r(char *restrict s1, char const *restrict s2, char **restrict rest);

size_t strxfrm_l(char *restrict s1, char const *restrict s2, size_t, locale_t);
