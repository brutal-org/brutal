#include <ahci/device.h>
#include <ahci/fis.h>
#include <brutal-debug>

static void init_hba_command_header(HbaCommand *command, bool write, uint32_t fis_len)
{
    command->command_len = fis_len / sizeof(uint32_t);

    command->write = write;
    command->prefetchable = write;

    command->clear_on_ok = 1;
    command->physical_region_descriptor_count = 1;
}

static void init_hba_command_table(AhciCommand *command, uintptr_t data_phys_addr, size_t count)
{
    mem_set((void *)command->table, 0, sizeof(HbaCommandTable) + (command->command->physical_region_descriptor_count) * sizeof(HbaPrdtEntry));

    command->table->entry[0].data_base_low = data_phys_addr;
    command->table->entry[0].data_base_up = data_phys_addr >> 32;
    command->table->entry[0].interrupt_on_complete = 1;
    command->table->entry[0].dbc = (count * 512) - 1;
}

static void init_fis_reg_command(FisHost2Dev *command, bool write, uint64_t cursor, size_t count)
{
    command->fis_type = FIS_TYPE_REG_HOST2DEV;
    command->ctrl_or_cmd = 1;

    if (write)
    {
        command->command = FIS_CMD_WRITE_DMA_EXT;
    }
    else
    {
        command->command = FIS_CMD_READ_DMA_EXT;
    }

    command->lba[0] = (cursor >> 0) & 0xff;
    command->lba[1] = (cursor >> 8) & 0xff;
    command->lba[2] = (cursor >> 16) & 0xff;
    command->device = 1 << 6;
    command->lba2[0] = (cursor >> 24) & 0xff;
    command->lba2[1] = (cursor >> 32) & 0xff;
    command->lba2[2] = (cursor >> 40) & 0xff;
    command->count_l = count;
    command->count_h = count >> 8;
}

static void ahci_wait_busy_drq(HbaPort *port)
{
    size_t time_out = 100000;

    while ((port->task_file_data & (HBA_DEVICE_BUSY | HBA_DEVICE_DRQ)))
    {
        time_out--;
        if (time_out == 0)
        {
            panic$("ahci device timed out !");
        }
    }
}

static int ahci_find_command_port(HbaPort *port)
{
    uint32_t slot = port->sata_active;

    for (int i = 0; i < 32; i++)
    {
        if ((slot & 1) == 0)
        {
            return i;
        }

        slot >>= 1;
    }

    panic$("can't find free command slot!");
    return -1;
}

// wait for the device to stop running and start a command
static void ahci_device_start_command(HbaPort *port)
{
    port->command_and_status &= ~((uint32_t)HBA_CMD_START);
    while (port->command_and_status & HBA_CMD_CMD_LIST_RUNNING)
        ;

    port->command_and_status |= HBA_CMD_FIS_RECEIVE_ENABLE;
    port->command_and_status |= HBA_CMD_START;
}

// stop the command and the fis receiving
static void ahci_device_end_command(HbaPort *port)
{
    port->command_and_status &= ~((uint32_t)HBA_CMD_START);

    while (port->command_and_status & HBA_CMD_CMD_LIST_RUNNING)
        ;

    port->command_and_status &= ~((uint32_t)HBA_CMD_FIS_RECEIVE_ENABLE);
}

static void ahci_device_init_command(AhciDevice *self)
{
    bal_mem_init(&self->command_mem, sizeof(HbaCommand) * 32);
    bal_mem_map(&self->command_mem);

    uintptr_t commands_padddr = 0;
    bal_mem_paddr(&self->command_mem, &commands_padddr);

    self->port->command_list_addr_low = commands_padddr;
    self->port->command_list_addr_up = commands_padddr >> 32;

    bal_mem_init(&self->fis_mem, sizeof(Fis));
    bal_mem_map(&self->fis_mem);

    Fis *fis = (Fis *)self->fis_mem.buf;
    *fis = (Fis){};
    fis->dma_setup_fis.fis_type = FIS_TYPE_DMA_SETUP;
    fis->pio_setup_fis.fis_type = FIS_TYPE_PIO_SETUP;
    fis->d2h_fis.fis_type = FIS_TYPE_REG_DEV2HOST;
    fis->sdbfis[0] = FIS_TYPE_DEV_BITS;

    uintptr_t fis_paddr = 0;
    bal_mem_paddr(&self->fis_mem, &fis_paddr);

    self->port->fis_addr_low = fis_paddr;
    self->port->fis_addr_up = fis_paddr >> 32;

    HbaCommand *commands = self->command_mem.buf;

    for (int i = 0; i < 32; i++)
    {
        self->commands[i].command = &commands[i];
        commands[i].physical_region_descriptor_count = 8;

        bal_mem_init(&self->commands[i].mem, 256);
        bal_mem_map(&self->commands[i].mem);

        self->commands[i].table = self->commands->mem.buf;

        uintptr_t command_padddr = 0;
        bal_mem_paddr(&self->commands[i].mem, &command_padddr);

        commands[i].command_table_descriptor_low = command_padddr;
        commands[i].command_table_descriptor_up = command_padddr >> 32;
    }
}

static void ahci_device_dump(AhciDevice *self)
{
    log$("Device {}", self->idx);
    log$("Interrupt-status: {#b}", self->port->interrupt_status);
    log$("Sata-status: {#b}", self->port->sata_status);
    log$("Sata-error: {#b}", self->port->sata_error);
    log$("Command & Status: {#b}", self->port->command_and_status);
    log$("Task-file: {#b}", self->port->task_file_data);
}

void ahci_device_init(AhciDevice *device, HbaPort *port, int idx)
{
    *device = (AhciDevice){};
    device->idx = idx;
    device->port = port;

    if (device->port->command_and_status & (HBA_CMD_CMD_LIST_RUNNING | HBA_CMD_START | HBA_CMD_FIS_RECEIVE_RUNNING | HBA_CMD_FIS_RECEIVE_ENABLE))
    {
        log$("Idling device");
        device->port->command_and_status &= ~((uint32_t)(HBA_CMD_START | HBA_CMD_FIS_RECEIVE_ENABLE));

        while (device->port->command_and_status & (HBA_CMD_CMD_LIST_RUNNING))
        {
        }
    }

    // turn on
    device->port->sata_control |= HBA_SCONTROL_RESET;
    device->port->command_and_status |= HBA_CMD_ICC_SET_ACTIVE;

    ahci_device_init_command(device);
}

bool ahci_device_rw(AhciDevice *self, BalMem *target, uint64_t cursor, uint64_t count, bool write)
{
    self->port->interrupt_status = 0xFFFFFFFF;

    int command_slot = ahci_find_command_port(self->port);

    if (command_slot == -1)
    {
        return false;
    }

    HbaCommand *cmd = self->commands[command_slot].command;

    init_hba_command_header(cmd, write, sizeof(FisHost2Dev));

    uintptr_t physical_addr = 0;
    bal_mem_paddr(target, &physical_addr);
    init_hba_command_table(&self->commands[command_slot], physical_addr, count);

    FisHost2Dev *command_fis = (FisHost2Dev *)self->commands[command_slot].table->command_fis;

    init_fis_reg_command(command_fis, write, cursor, count);

    ahci_wait_busy_drq(self->port);

    ahci_device_start_command(self->port);

    self->port->command_issue |= 1 << command_slot;

    while (true)
    {
        if (!(self->port->command_issue & (1 << command_slot)))
        {
            break;
        }

        if (self->port->interrupt_status & HBA_INT_TASK_FILE_ERROR_STATUS)
        {
            break;
        }
    }

    if (self->port->interrupt_status & HBA_INT_TASK_FILE_ERROR_STATUS)
    {
        ahci_device_dump(self);
        panic$("task error while {} at: {} count: {}", write ? "writing" : "reading", cursor, count);
    }

    ahci_device_end_command(self->port);

    return true;
}
