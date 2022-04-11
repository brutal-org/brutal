#pragma once

#include <bal/boot.h>
#include "system/unit.h"

typedef struct
{
    IpcObject base;
    Vec(Unit) units;
    Vec(IpcCap) caps;
} Bus;

void bus_init(Bus *self, Alloc *alloc);

void bus_deinit(Bus *self);

void bus_expose(Bus *self, IpcCap cap);

IpcCap bus_consume(Bus *self, IpcProto proto);

void bus_activate(Bus *self, Unit *unit);
