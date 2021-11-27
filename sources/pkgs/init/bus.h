#pragma once

#include <bal/task.h>
#include <brutal/ds.h>

typedef struct
{
    Handover *handover;
    Vec(BalTask) serv;
} Bus;

void bus_init(Bus *bus, Handover *handover, Alloc *alloc);

void bus_deinit(Bus *bus);

BrId bus_lookup(Bus *bus, Str name);

BrId bus_start(Bus *bus, Str name, BalArgs args);
