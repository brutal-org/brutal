#define I_M_A_BAD_PROGRAMMER_OR_I_M_WRITING_A_C_LIBRARY

/* --- 7.24 - String handling ----------------------------------------------- */

#include <brutal/alloc.h>
#include <brutal/mem.h>
#include <string.h>

/* --- 7.24.2 - Copying functions ------------------------------------------- */

void *memcpy(void *restrict s1, void const *restrict s2, size_t n)
{
    return mem_cpy(s1, s2, n);
}

void *memmove(void *s1, void const *s2, size_t n)
{
    char const *usrc = (char const *)s2;
    char *udest = (char *)s1;

    if (udest < usrc)
    {
        for (size_t i = 0; i < n; i++)
        {
            udest[i] = usrc[i];
        }
    }
    else if (udest > usrc)
    {
        for (size_t i = n; i > 0; i--)
        {
            udest[i - 1] = usrc[i - 1];
        }
    }

    return s1;
}

char *strcpy(char *restrict s1, char const *restrict s2)
{
    size_t i = 0;

    while (s2[i] != 0)
    {
        s1[i] = s2[i];
        i++;
    }

    return s1;
}

char *strncpy(char *restrict s1, char const *restrict s2, size_t n)
{
    size_t i = 0;

    while (s2[i] != 0 && i < n)
    {
        s1[i] = s2[i];
        i++;
    }

    return s1;
}

/* --- 7.24.3 - Concatenation functions ------------------------------------- */

// char *strcat(char *restrict s1,  char const *restrict s2) {}

//char *strncat(char *restrict s1,  char const *restrict s2, size_t n) {}

/* --- 7.24.4 - Comparison functions ---------------------------------------- */

int memcmp(void const *s1, const void *s2, size_t n)
{
    for (size_t i = 0; i < n; i++)
    {
        int diff = *(char *)s1 - *(char *)s2;
        if (diff != 0)
        {
            return diff;
        }
    }

    return 0;
}

int strcmp(char const *s1, char const *s2)
{
    while (*s1 == *s2 && *s1)
    {
        s1++;
        s2++;
    };

    return (int)*s1 - (int)*s2;
}

// int strcoll(char const *s1, char const*s2) {}

int strncmp(char const *s1, char const *s2, size_t n)
{
    while (*s1 == *s2 && *s1 && n)
    {
        s1++;
        s2++;
        n--;
    }

    if (n == 0)
    {
        return 0;
    }

    return (int)*s1 - (int)*s2;
}

// size_t strxfrm(char *restrict s1, char const *restrict s2, size_t n) {}

/* --- 7.24.5 - Search functions -------------------------------------------- */

// void *memchr(void const *s, int c, size_t n) {}

// char *strchr(char const *s, int c) {}

// size_t strcspn(char const *s1,  char const *s2) {}

// char *strpbrk(char const *s1,  char const *s2) {}

char *strrchr(char const *s, int c)
{
    size_t length = strlen(s);
    size_t pos = length;

    while (s[pos] != c && pos-- != 0)
        ;

    if (pos == length)
    {
        return NULL;
    }

    return (char *)s + pos;
}

// size_t strspn(char const *s1,  char const *s2) {}

// char *strstr(char const *s1,  char const *s2) {}

// char *strtok(char *restrict s1,  char const *restrict s2) {}

/* --- 7.24.6 - Miscellaneous functions ------------------------------------- */

void *memset(void *s, int c, size_t n)
{
    return mem_set(s, c, n);
}

// char *strerror(int errnum) { }

size_t strlen(char const *s)
{
    size_t i = 0;

    while (s[i++] != '\0')
        ;

    return i;
}
