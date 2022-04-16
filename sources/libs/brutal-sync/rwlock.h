#pragma once

#include "lock.h"

typedef struct
{
    Lock lock;

    _Atomic int pendings;
    int readers;
    int writers;
} RwLock;

void rwlock_init(RwLock *lock);

void rwlock_deinit(RwLock *lock);

void rwlock_acquire_read(RwLock *lock);

bool rwlock_try_acquire_read(RwLock *lock);

void rwlock_release_read(RwLock *lock);

void rwlock_acquire_write(RwLock *lock);

bool rwlock_try_acquire_write(RwLock *lock);

void rwlock_release_write(RwLock *lock);
