#include <errno.h>

int _errno_value = 0;

int *_errno(void)
{
    return &_errno_value;
}
