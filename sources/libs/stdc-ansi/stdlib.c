#include <embed/log.h>
#include <embed/task.h>
#include <stdlib.h>
#include <string.h>
#include <brutal-alloc>
#include <brutal-debug>
#include <brutal-math>
#include <brutal-mem>
#include <brutal-parse>

/* --- 7.22 - General utilities --------------------------------------------- */

/* --- 7.22.1 - Numeric conversion functions -------------------------------- */

double atof(char const *nptr)
{
    double res = 0;
    str_to_float(str$(nptr), &res);
    return res;
}

int atoi(char const *nptr)
{
    long res = 0;
    str_to_int(str$(nptr), &res);
    return res;
}

long atol(char const *nptr)
{
    long res = 0;
    str_to_int(str$(nptr), &res);
    return res;
}

long long atoll(char const *nptr)
{
    long res = 0;
    str_to_int(str$(nptr), &res);
    return res;
}

// long int strtol(char const *restrict nptr,
//                 char **restrict endptr,
//                 int base);

// long long int strtoll(char const *restrict nptr,
//                       char **restrict endptr,
//                       int base);

// unsigned long int strtoul(char const *restrict nptr,
//                           char **restrict endptr,
//                           int base);

// unsigned long long int strtoull(char *const restrict nptr,
//                                 char **restrict endptr,
//                                 int base);

// double strtod(char const *restrict nptr,
//               char **restrict endptr);

// float strtof(char const *restrict nptr,
//              char **restrict endptr);

// long double strtold(char const *restrict nptr, char **restrict endptr);

// int strfromd(char *restrict s,
//              size_t n,
//              char const *restrict format,
//              double fp);

// int strfromf(char *restrict s,
//              size_t n,
//              char const *restrict format,
//              floatf p);

// int strfroml(char *restricts,
//              size_t n,
//              char const *restrict format,
//              long double fp);

/* --- 7.22.2 - Pseudo-random sequence generation functions ----------------- */

static MRand _rand = m_rand_init$(123456789);

int rand()
{
    return m_rand_next_u32(&_rand);
}

void srand(unsigned int seed)
{
    m_rand_init(&_rand, seed);
}

/* --- 7.22.3 - Memory management functions --------------------------------- */

void *aligned_alloc(size_t alignment, size_t size)
{
    if (alignment == 16)
    {
        panic$("aligned_alloc: alignment = {} is not supported!", alignment);
    }

    return malloc(size);
}

void *calloc(size_t nmemb, size_t size)
{
    return alloc_calloc(alloc_global(), nmemb, size);
}

void free(void *ptr)
{
    alloc_free(alloc_global(), ptr);
}

void *malloc(size_t size)
{
    return aligned_alloc(16, size);
}

void *realloc(void *ptr, size_t size)
{
    return alloc_resize(alloc_global(), ptr, size);
}

/* --- 7.22.4 - Communication with the environment -------------------------- */

_Noreturn void abort(void)
{
    embed_log_panic();
}

// int atexit(void (*func)(void));

// int at_quick_exit(void (*func)(void));

_Noreturn void exit(int status)
{
    embed_task_exit(embed_task_self(), status);
    embed_log_panic();
}

_Noreturn void _Exit(int status)
{
    embed_task_exit(embed_task_self(), status);
    embed_log_panic();
}

// char *getenv(char const *name);

// _Noreturn void quick_exit(int status);

// int system(char const *string);

/* --- 7.22.5 - Searching and sorting utilities ----------------------------- */

// void *bsearch(void const *key,
//               void const *base,
//               size_t nmemb,
//               size_t size,
//               int (*compar)(void const *, void const *));

// void qsort(void *base, size_t nmemb, size_t size, int (*compar)(void const *, void const *))
// In qsort.c

/* --- 7.22.6 - Integer arithmetic functions -------------------------------- */

int abs(int j)
{
    if (j < 0)
    {
        return -j;
    }

    return j;
}

long labs(long j)
{
    if (j < 0)
    {
        return -j;
    }

    return j;
}

long long llabs(long long j)
{
    if (j < 0)
    {
        return -j;
    }

    return j;
}

div_t div(int number, int denom)
{
    div_t res = {
        .quot = number / denom,
        .rem = number % denom,
    };

    return res;
}

ldiv_t ldiv(long number, long denom)
{
    ldiv_t res = {
        .quot = number / denom,
        .rem = number % denom,
    };

    return res;
}

lldiv_t lldiv(long long number, long long denom)
{
    lldiv_t res = {
        .quot = number / denom,
        .rem = number % denom,
    };

    return res;
}

/* --- 7.22.7 - Multibyte/wide character conversion functions --------------- */

// int mblen(char const *s, size_t n);

// int mbtowc(wchar_t *restrict pwc, char const *restrict s, size_t n);

// int wctomb(char *s, wchar_t wc);

// size_t mbstowcs(wchar_t *restrict pwcs, char const *restrict s, size_t n);

// size_t mcstombs(char *restrict s, wchar_t const *restrict pwcs, size_t n);
