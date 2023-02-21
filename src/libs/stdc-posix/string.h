#pragma once

#include <stdc-base/prelude.h>
#include <locale/local_t.h>
#include <brutal/base/attributes.h>
#include <brutal/base/std.h>



void *memccpy(void *STDC_RESTRICT s1, void const *STDC_RESTRICT s2, int c, size_t n);

char *stpcpy(char *STDC_RESTRICT s1, char const *STDC_RESTRICT s2);

char *stpncpy(char *STDC_RESTRICT s1, char const *STDC_RESTRICT s2, size_t n);

int strcoll_l(char const *, char const *, locale_t);

char *strdup(char const *);

char *strerror_l(int, locale_t);

int strerror_r(int, char *, size_t);

char *strndup(char const *, size_t);

size_t strnlen(char const *, size_t);

char *strsignal(int);

char *strtok_r(char *STDC_RESTRICT s1, char const *STDC_RESTRICT s2, char **STDC_RESTRICT rest);

size_t strxfrm_l(char *STDC_RESTRICT s1, char const *STDC_RESTRICT s2, size_t, locale_t);
