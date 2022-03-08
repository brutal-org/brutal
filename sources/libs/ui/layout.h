#pragma once

#include <brutal/ds.h>
#include <brutal/ui/layout.h>

typedef struct
{
    int child;
    int next;
    MRectf bound;
    UiLayout layout;
} Lay;

typedef struct
{
    Vec(Lay);
} LayTree;

typedef struct
{
    LayTree tree;
    Vec(int) stack;
} LayBuilder;
