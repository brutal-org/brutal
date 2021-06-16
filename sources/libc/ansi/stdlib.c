#include <stdlib.h>

// --- environment communication ---

// _Noreturn void abort(void);

// int atexit(void (*func)(void));

// int at_quick_exit(void (*func)(void));

// _Noreturn void exit(int status);

// _Noreturn void _Exit(int status);

// char *getenv(char const *name);

// _Noreturn void quick_exit(int status);

// int system(char const *string);

// --- number / random function ---

// int abs(int j);

// long labs(long j);

// long long llabs(long long j);

// int rand();

// int srand(unsigned int seed);

// div_t div(int number, int denom);

// ldiv_t ldiv(long number, long denom);

// lldiv_t lldiv(long long number, long long denom);

// --- str utils function ---

// double atof(char const *nptr);

// int atoi(char const *nptr);

// long atol(char const *nptr);

// long long atoll(char const *nptr);

// long int strtol(char const *restrictnptr, char **restrictendptr, int base);

// long long int strtoll(char const *restrictnptr, char **restrictendptr, int base);

// unsigned long int strtoul(char const *restrictnptr, char **restrictendptr, int base);

// unsigned long long int strtoull(char *const restrictnptr, char **restrictendptr, int base);

// double strtod(char const *restrictnptr, char **restrictendptr);

// float strtof(char const *restrictnptr, char **restrictendptr);

// long double strtold(char const *restrictnptr, char **restrictendptr);

// int mblen(char const *s, size_t n);

// int mbtowc(wchar_t *restrict pwc, char const *restrict s, size_t n);

// int wctomb(char *s, wchar_t wc);

// size_t mbstowcs(wchar_t *restrict pwcs, char const *restrict s, size_t n);

// size_t mcstombs(char *restrict s, wchar_t const *restrict pwcs, size_t n);

// --- array utils ---

// void *bsearch(void const *key, void const *base, size_t nmemb, size_t size, int (*compar)(void const *, void const *));

// void *qsort(void *base, size_t nmemb, size_t size, int (*compar)(void const *, void const *));

// --- memory utils ---

// void *aligned_alloc(size_t alignment, size_t size);

// void *calloc(size_t nmemb, size_t size);

// void free(void *ptr);

// void *malloc(size_t size);

// void *realloc(void *ptr, size_t size);
// --- print utils ---

// int strfromd(char *restricts, size_t n, char const *restrictformat, double fp);

// int strfromf(char *restricts, size_t n, char const *restrictformat, floatf p);

// int strfroml(char *restricts, size_t n, char const *restrictformat, long double fp);
