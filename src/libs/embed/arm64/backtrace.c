#include <embed/debug.h>
#include <brutal-base/attributes.h>

struct _StackFrame
{
    struct _StackFrame *rbp;
    uint64_t rip;
};

size_t embed_debug_backtrace(uintptr_t *buf, MAYBE_UNUSED size_t cap)
{
    buf[0] = 0;
    return 1;
}
