#include <embed/debug.h>

size_t embed_debug_backtrace(MAYBE_UNUSED uintptr_t *buf, MAYBE_UNUSED size_t cap)
{
    buf[0] = 0;
    return 1;
}
