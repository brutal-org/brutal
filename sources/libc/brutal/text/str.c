#include <brutal/base/keywords.h>
#include <brutal/text/str.h>

void str_rvs(str_t str)
{
    for (int i = 0, j = str.len - 1; i < j; i++, j--)
    {
        auto c = str.buffer[i];
        str.buffer[i] = str.buffer[j];
        str.buffer[j] = c;
    }
}

bool str_eq(str_t lhs, str_t rhs)
{
    if (lhs.len != rhs.len)
    {
        return false;
    }

    for (size_t i = 0; i < MIN(lhs.len, rhs.len); i++)
    {
        if (lhs.buffer[i] != rhs.buffer[i])
        {
            return false;
        }
    }

    return true;
}
