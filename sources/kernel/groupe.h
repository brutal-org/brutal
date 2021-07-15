#pragma once

#include <syscalls/types.h>

typedef struct group Group;

struct group
{
    BrGroupeFlags flags;

    Group *parent;
};
