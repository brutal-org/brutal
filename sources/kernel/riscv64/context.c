#include <brutal/alloc.h>
#include <brutal/debug.h>
#include "kernel/context.h"

Context *context_create(void)
{
    Context *self = (Context *)alloc_malloc(alloc_global(), sizeof(Context));
    return self;
}

void context_destroy(Context *self)
{
    alloc_free(alloc_global(), self);
}

void context_start(MAYBE_UNUSED Context *self, MAYBE_UNUSED uintptr_t ip, MAYBE_UNUSED uintptr_t sp, MAYBE_UNUSED uintptr_t ksp, MAYBE_UNUSED BrTaskArgs args, MAYBE_UNUSED BrTaskFlags flags)
{
}
