#pragma once

#include <ahci/hba.h>
#include <bal/hw.h>

typedef struct
{
    HbaCommand *command;
    HbaCommandTable *table;
    BalMem mem;
} AhciCommand;

typedef struct
{
    int idx;
    HbaPort *port;
    BalMem command_mem;
    BalMem fis_mem;
    AhciCommand commands[32];
    BalMem identify;
} AhciDevice;

void ahci_device_init(AhciDevice *device, HbaPort *port, int idx);

void ahci_device_start_command(AhciDevice *self);

void ahci_device_end_command(AhciDevice *self);

bool ahci_device_io_command(AhciDevice *self, BrMemObj target, uint64_t cursor, uint64_t count, bool write);
