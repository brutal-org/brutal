#include <brutal/base/keywords.h>
#include <brutal/text/str.h>

void str_rvs(Str str)
{
    for (int i = 0, j = str.len - 1; i < j; i++, j--)
    {
        char c = str.buffer[i];
        str.buffer[i] = str.buffer[j];
        str.buffer[j] = c;
    }
}

Str str_dup(Str const str, Alloc *alloc)
{
    if (str.len == 0)
    {
        return str_make_from_cstr("");
    }
    char *buffer = (char *)alloc_malloc(alloc, str.len);
    mem_cpy(buffer, str.buffer, str.len);
    return str_n$(str.len, buffer);
}

bool str_eq(Str const lhs, Str const rhs)
{
    if (lhs.len != rhs.len)
    {
        return false;
    }

    for (size_t i = 0; i < lhs.len; i++)
    {
        if (lhs.buffer[i] != rhs.buffer[i])
        {
            return false;
        }
    }

    return true;
}

int str_count(Str const haystack, Str const needle)
{
    if (haystack.len < needle.len)
    {
        return 0;
    }

    int count = 0;
    for (size_t i = 0; i < haystack.len - needle.len; i++)
    {
        Str slice = str_n$(needle.len, haystack.buffer + i);

        if (str_eq(slice, needle))
        {
            count++;
        }
    }

    return count;
}

int str_count_chr(Str const str, char chr)
{
    int result = 0;

    for (size_t i = 0; i < str.len; i++)
    {
        if (str.buffer[i] == chr)
        {
            result++;
        }
    }

    return result;
}

int str_last(Str const lStr, Str const rStr)
{
    if (lStr.len < rStr.len)
    {
        return 0;
    }

    int pos = -1;
    for (size_t i = 0; i < lStr.len - rStr.len; i++)
    {
        Str substr = str_n$(rStr.len, lStr.buffer + i);

        if (str_eq(substr, rStr))
        {
            pos = i;
        }
    }

    return pos;
}

int str_last_chr(Str const str, char chr)
{
    int result = -1;

    for (size_t i = 0; i < str.len; i++)
    {
        if (str.buffer[i] == chr)
        {
            result = i;
        }
    }

    return result;
}

int str_first(Str const lStr, Str const rStr)
{
    if (lStr.len < rStr.len)
    {
        return 0;
    }

    for (size_t i = 0; i < lStr.len - rStr.len; i++)
    {
        Str substr = str_n$(rStr.len, lStr.buffer + i);

        if (str_eq(substr, rStr))
        {
            return i;
        }
    }

    return -1;
}

int str_first_chr(Str const str, char chr)
{
    for (size_t i = 0; i < str.len; i++)
    {
        if (str.buffer[i] == chr)
        {
            return i;
        }
    }

    return -1;
}
