#include "path.h"

Str io_dirname(Str path)
{
    size_t len = 0;

    for (size_t i = 0; i < path.len; i++)
    {
        if (path.buf[i] == '/')
        {
            len = i + 1;
        }
    }

    return str_n$(len, path.buf);
}

Str io_basename(Str path)
{
    size_t len = 0;

    for (size_t i = 0; i < path.len; i++)
    {
        if (path.buf[i] == '/')
        {
            len = i;
        }
    }

    return str_n$(len, path.buf);
}

Str io_filename(Str path)
{
    size_t start = 0;

    for (size_t i = 0; i < path.len; i++)
    {
        if (path.buf[i] == '/')
        {
            start = i + 1;
        }
    }

    return str_n$(path.len - start, path.buf + start);
}
