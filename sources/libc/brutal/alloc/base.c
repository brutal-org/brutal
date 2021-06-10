#include <brutal/alloc/base.h>

void alloc_no_op(struct alloc *self, void *ptr)
{
    UNUSED(self);
    UNUSED(ptr);
}
