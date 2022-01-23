#pragma once

#include <bal/boot.h>
#include <bal/task.h>
#include <brutal/ds.h>

typedef struct
{
    Str name;
    BrAddr addr;
} BbusComponent;

typedef struct
{
    Handover *handover;
    Vec(BbusComponent) serv;
} Bus;

void bus_init(Bus *bus, Handover *handover, Alloc *alloc);

void bus_deinit(Bus *bus);

BrAddr bus_lookup(Bus *bus, Str name);

void bus_start(Bus *bus, Str name, IpcCap *caps, size_t len);
