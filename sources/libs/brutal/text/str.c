#include <brutal/base/keywords.h>
#include <brutal/text/str.h>

void str_rvs(Str str)
{
    for (int i = 0, j = str.len - 1; i < j; i++, j--)
    {
        auto c = str.buffer[i];
        str.buffer[i] = str.buffer[j];
        str.buffer[j] = c;
    }
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
        auto slice = str_cast_n(needle.len, haystack.buffer + i);

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
        auto offseted_lStr = str_cast_n(rStr.len, lStr.buffer + i);

        if (str_eq(offseted_lStr, rStr))
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
        auto offseted_lStr = str_cast_n(rStr.len, lStr.buffer + i);

        if (str_eq(offseted_lStr, rStr))
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
