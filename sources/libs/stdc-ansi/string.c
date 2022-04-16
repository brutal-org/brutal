#define _BRUTAL_ALLOW_BANNED

/* --- 7.24 - String handling ----------------------------------------------- */

#include <string.h>
#include <brutal-alloc>
#include <brutal-mem>

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

char *strcat(char *restrict s1, char const *restrict s2)
{
    size_t l2 = strlen(s2);
    s1 = s1 + strlen(s1); // append to the last

    for (size_t i = 0; i < l2; i++)
    {
        s1[i] = s2[i];
    }

    s1[l2] = 0;

    return s1;
}

char *strncat(char *restrict s1, char const *restrict s2, size_t n)
{
    size_t l2 = strlen(s2);
    size_t i = 0;
    s1 = s1 + strlen(s1);

    for (i = 0; i < l2 && i < n; i++)
    {
        s1[i] = s2[i];
    }

    s1[i] = 0;

    return s1;
}

/* --- 7.24.4 - Comparison functions ---------------------------------------- */

int memcmp(void const *s1, void const *s2, size_t n)
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

char *strchr(char const *s, int c)
{
    size_t len = strlen(s);
    for (size_t i = 0; i < len; i++)
    {
        if (s[i] == c)
        {
            return (char *)s + i;
        }
    }

    return nullptr;
}

// size_t strcspn(char const *s1,  char const *s2) {}

// char *strpbrk(char const *s1,  char const *s2) {}

char *strrchr(char const *s, int c)
{
    size_t len = strlen(s);
    size_t pos = len;

    while (s[pos] != c && pos-- != 0)
        ;

    if (pos == len)
    {
        return nullptr;
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
