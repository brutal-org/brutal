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

bool str_eq(const Str lhs, const Str rhs)
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

int str_cnt(const Str lStr, const Str rStr)
{

    if (lStr.len < rStr.len)
    {
        return 0;
    }

    int count = 0;
    for (size_t i = 0; i < lStr.len - rStr.len; i++)
    {
        auto offseted_lStr = str_cast_n(rStr.len, lStr.buffer + i);

        if (str_eq(offseted_lStr, rStr))
        {
            count++;
        }
    }

    return count;
}

int str_last(const Str lStr, const Str rStr)
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

int str_first(const Str lStr, const Str rStr)
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
