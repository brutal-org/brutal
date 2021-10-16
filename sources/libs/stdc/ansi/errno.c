#include <errno.h>

int _errno = 0;

int *_get_errno(void)
{
    return &_errno;
}
