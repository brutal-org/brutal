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

#define AHCI_WRITE true
#define AHCI_READ false

void ahci_device_init(AhciDevice *device, HbaPort *port, int idx);

bool ahci_device_rw(AhciDevice *self, BalMem *target, uint64_t cursor, uint64_t count, bool write);
