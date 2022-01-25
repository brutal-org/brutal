#pragma once

#include <bal/boot.h>
#include <brutal/ds.h>
#include <ipc/ipc.h>

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
    Vec(IpcCap) consume;
} Unit;

void unit_init(Unit *self, Str name, Alloc *alloc);

void unit_deinit(Unit *self);

void unit_consume(Unit *self, IpcProto proto);

void unit_provide(Unit *self, IpcCap cap);

void unit_start(Unit *self, Handover *handover);
