#include "kernel/context.h"
#include <brutal-alloc>
#include <brutal-debug>

Context *context_create(void)
{
    Context *self = alloc_make(alloc_global(), Context);
    return self;
}

void context_destroy(Context *self)
{
    alloc_free(alloc_global(), self);
}

void context_start(MAYBE_UNUSED Context *self, MAYBE_UNUSED uintptr_t ip, MAYBE_UNUSED uintptr_t sp, MAYBE_UNUSED uintptr_t ksp, MAYBE_UNUSED BrTaskArgs args, MAYBE_UNUSED BrTaskFlags flags)
{
    panic$("context_start not implemented");
}
