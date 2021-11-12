#include "embed/arch.h"

struct stackframe
{
    struct stackframe *rbp;
    uint64_t rip;
};

size_t arch_backtrace(uintptr_t *buf, MAYBE_UNUSED size_t cap)
{
    buf[0] = 0;
    return 1;
}
