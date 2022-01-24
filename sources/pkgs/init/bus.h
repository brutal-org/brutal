#pragma once

#include <bal/boot.h>
#include <bal/task.h>
#include <brutal/ds.h>

typedef struct
{
    Handover *handover;
    Vec(IpcCap) caps;
} Bus;

void bus_init(Bus *bus, Handover *handover, Alloc *alloc);

void bus_deinit(Bus *bus);

IpcCap bus_lookup(Bus *bus, IpcProto proto);

void bus_start(Bus *bus, Str name, IpcCap *caps, size_t len);
