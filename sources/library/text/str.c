#include <library/text/str.h>

void str_rvs(str_t str)
{
    for (int i = 0, j = str.len - 1; i < j; i++, j--)
    {
        auto c = str.buffer[i];
        str.buffer[i] = str.buffer[j];
        str.buffer[j] = c;
    }
}