#include <brutal/debug.h>
#include <embed/posix/err.h>
#include <errno.h>
#include <string.h>

Error error_from_errno(void)
{
    if (errno == 0)
    {
        return ERR_SUCCESS;
    }
    else
    {
        log$("{}", strerror(errno));
        return make_error(ERR_KIND_UNDEFINED, strerror(errno));
    }
}
