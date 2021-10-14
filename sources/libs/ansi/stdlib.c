#include <brutal/alloc.h>
#include <brutal/host.h>
#include <brutal/log.h>
#include <brutal/mem.h>
#include <stdlib.h>

/* --- 7.22 - General utilities --------------------------------------------- */

/* --- 7.22.1 - Numeric conversion functions -------------------------------- */

// double atof(char const *nptr);

int atoi(char const *nptr)
{
    int res = 0;

    for(int i = 0; nptr[i] != 0; i++)
    {
        if(isdigit(nptr[i]) == 0)
        {
            return 0;
        }
        res = res * 10 + nptr[i] - '0'; 
    }

    return res;
}

long atol(char const *nptr)
{
    long res = 0;

    for(int i = 0; nptr[i] != 0; i++)
    {
        if(isdigit(nptr[i]) == 0)
        {
            return 0;
        }
        res = res * 10 + nptr[i] - '0'; 
    }

    return res;
}

long long atoll(char const *nptr)
{
    long long res = 0;

    for(int i = 0; nptr[i] != 0; i++)
    {
        if(isdigit(nptr[i]) == 0)
        {
            return 0;
        }
        res = res * 10 + nptr[i] - '0'; 
    }

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

static unsigned long next_n = 1;

int rand()
{
    next_n = next_n * 1103515245 + 12345;
    return ((unsigned)(next_n / 65536) % 32768);
}

void srand(unsigned int seed)
{
    next_n = seed;
}

/* --- 7.22.3 - Memory management functions --------------------------------- */

static HeapAlloc _heap;
static bool _heap_init = false;

static Alloc *heap(void)
{
    if (!_heap_init)
    {
        heap_alloc_init(&_heap, NODE_DEFAULT);
        _heap_init = true;
    }

    return base$(&_heap);
}

void *aligned_alloc(size_t alignment, size_t size)
{
    if (alignment > 16)
    {
        panic$("aligned_alloc: alignment = {} is not supported!", alignment);
    }

    return malloc(size);
}

void *calloc(size_t nmemb, size_t size)
{
    return alloc_calloc(heap(), nmemb, size);
}

void free(void *ptr)
{
    alloc_free(heap(), ptr);
}

void *malloc(size_t size)
{
    return alloc_malloc(heap(), size);
}

void *realloc(void *ptr, size_t size)
{
    return alloc_resize(heap(), ptr, size);
}

/* --- 7.22.4 - Communication with the environment -------------------------- */

_Noreturn void abort(void)
{
    host_log_panic();
}

// int atexit(void (*func)(void));

// int at_quick_exit(void (*func)(void));

_Noreturn void exit(int status)
{
    host_task_exit(host_task_self(), status);
    host_log_panic();
}

_Noreturn void _Exit(int status)
{
    host_task_exit(host_task_self(), status);
    host_log_panic();
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

void qsort(void *base, size_t nmemb, size_t size, int (*compar)(void const *, void const *))
{
    // FIXME: Write real quick sort

    for (size_t i = 0; i + 1 < nmemb; i++)
    {
        for (size_t j = i + 1; i < nmemb; i++)
        {
            void *a = base + (i * size);
            void *b = base + (j * size);

            if (compar(a, b) < 0)
            {
                char tmp[size];
                mem_cpy(tmp, a, size);
                mem_cpy(a, b, size);
                mem_cpy(b, tmp, size);
            }
        }
    }
}

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

// div_t div(int number, int denom);

// ldiv_t ldiv(long number, long denom);

// lldiv_t lldiv(long long number, long long denom);

/* --- 7.22.7 - Multibyte/wide character conversion functions --------------- */

// int mblen(char const *s, size_t n);

// int mbtowc(wchar_t *restrict pwc, char const *restrict s, size_t n);

// int wctomb(char *s, wchar_t wc);

// size_t mbstowcs(wchar_t *restrict pwcs, char const *restrict s, size_t n);

// size_t mcstombs(char *restrict s, wchar_t const *restrict pwcs, size_t n);
