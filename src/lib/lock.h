#pragma once

#include <base-std.h>

struct lock
{
    int locked;
};

void lock_acquire(struct lock *lock);

void lock_release(struct lock *lock);