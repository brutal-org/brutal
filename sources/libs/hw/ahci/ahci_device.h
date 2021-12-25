#pragma once
#include <bal/hw/mem.h>
#include <hw/ahci/hba.h>

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

void init_ahci_device(AhciDevice *device, HbaPort *port, int idx);

void start_ahci_device_command(AhciDevice *self);
void end_ahci_device_command(AhciDevice *self);

bool ahci_io_cmd(AhciDevice *self, BrMemObj target, uint64_t cursor, uint64_t count, bool write);
