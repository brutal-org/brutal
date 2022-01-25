#pragma once

#include <bal/boot.h>
#include "init/unit.h"

typedef struct
{
    Handover *handover;
    Vec(Unit) units;
    Vec(IpcCap) caps;
} Bus;

void bus_init(Bus *self, Handover *handover, Alloc *alloc);

void bus_deinit(Bus *self);

void bus_expose(Bus *self, IpcCap cap);

IpcCap bus_consume(Bus *self, IpcProto proto);

void bus_activate(Bus *self, Unit *unit);
