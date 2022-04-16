#pragma once

typedef enum
{
    ITER_STOP,
    ITER_CONTINUE,
} Iter;

typedef Iter IterFn(void *val, void *ctx);
