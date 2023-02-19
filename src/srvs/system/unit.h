#pragma once

#include <bal/boot.h>
#include <bal/hw.h>
#include <ipc/ipc.h>
#include <brutal-ds>

typedef enum
{
    UNIT_PENDING,
    UNIT_READY,
    UNIT_RUNNING,
} UnitState;

typedef struct
{
    UnitState state;
    Str name;
    int ready;
    BalMem payload;
    Vec(IpcCap) consume;
    BrRight rights;
} Unit;

void unit_init(Unit *self, Str name, BalMem payload, Alloc *alloc);

void unit_init_from_module(Unit *self, HandoverModule mod, Alloc *alloc);

void unit_deinit(Unit *self);

void unit_consume(Unit *self, IpcProto proto);

void unit_provide(Unit *self, IpcCap cap);

void unit_start(Unit *self);
