#include <ahci/ahci_device.h>
#include <ahci/fis.h>
#include <brutal/debug.h>

static void init_hba_command_header(HbaCommand *command, bool write, uint32_t fis_len)
{
    command->command_len = fis_len / sizeof(uint32_t);

    command->write = write;
    if (write)
    {
        command->prefetchable = 1;
        command->clear_on_ok = 1;
    }
    else
    {
        command->prefetchable = 0;
        command->clear_on_ok = 0;
    }
    command->physical_region_descriptor_count = 1;
}

static void init_hba_command_table(AhciCommand *command, uintptr_t data_phys_addr, size_t count)
{
    mem_set((void *)command->table, 0, sizeof(HbaCommandTable) + (command->command->physical_region_descriptor_count - 1) * sizeof(HbaPrdtEntry));

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

static void ahci_wait_busy_drq(AhciDevice *self)
{
    size_t time_out = 100000;

    while ((self->port->task_file_data & (HBA_DEVICE_BUSY | HBA_DEVICE_DRQ)))
    {
        time_out--;
        if (time_out == 0)
        {
            panic$("ahci device timed out !");
        }
    }
}

static int ahci_find_command_port(AhciDevice *self)
{
    uint32_t slot = self->port->sata_active;

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
void start_ahci_device_command(AhciDevice *self)
{
    self->port->command_and_status &= ~HBA_CMD_START;
    while (self->port->command_and_status & HBA_CMD_CMD_LIST_RUNNING)
        ;

    self->port->command_and_status |= HBA_CMD_FIS_RECEIVE_ENABLE;
    self->port->command_and_status |= HBA_CMD_START;
}
// stop the command and the fis receiving
void end_ahci_device_command(AhciDevice *self)
{
    self->port->command_and_status &= ~((uint32_t)HBA_CMD_START);

    while (self->port->command_and_status & HBA_CMD_CMD_LIST_RUNNING)
        ;

    self->port->command_and_status &= ~((uint32_t)HBA_CMD_FIS_RECEIVE_ENABLE);
}

static void init_ahci_device_command(AhciDevice *self)
{
    bal_mem_init_size(&self->command_mem, sizeof(HbaCommand) * 32);
    self->port->command_list_addr_low = self->command_mem.paddr;
    self->port->command_list_addr_up = self->command_mem.paddr >> 32;

    bal_mem_init_size(&self->fis_mem, sizeof(Fis));

    Fis *fis = (Fis *)self->fis_mem.buf;
    *fis = (Fis){};
    fis->dma_setup_fis.fis_type = FIS_TYPE_DMA_SETUP;
    fis->pio_setup_fis.fis_type = FIS_TYPE_PIO_SETUP;
    fis->d2h_fis.fis_type = FIS_TYPE_REG_DEV2HOST;
    fis->sdbfis[0] = FIS_TYPE_DEV_BITS;
    log$("fis addr: {}", self->fis_mem.paddr);
    self->port->fis_addr_low = self->fis_mem.paddr;
    self->port->fis_addr_up = self->fis_mem.paddr >> 32;

    HbaCommand *commands = self->command_mem.buf;

    for (int i = 0; i < 32; i++)
    {
        self->commands[i].command = &commands[i];
        commands[i].physical_region_descriptor_count = 8;

        bal_mem_init_size(&self->commands[i].mem, 256);
        self->commands[i].table = self->commands->mem.buf;

        commands[i].command_table_descriptor_low = self->commands[i].mem.paddr;
        commands[i].command_table_descriptor_up = self->commands[i].mem.paddr >> 32;
    }
}

bool ahci_identify(AhciDevice *self, uint8_t command)
{

    int command_slot = ahci_find_command_port(self);
    log$("using slot: {}", command_slot);
    if (command_slot == -1)
    {

        return false;
    }

    bal_mem_init_size(&self->identify, 512);
    HbaCommand *cmd = self->commands[command_slot].command;

    init_hba_command_header(cmd, false, sizeof(FisHost2Dev));
    init_hba_command_table(&self->commands[command_slot], self->command_mem.paddr, 1);

    volatile FisHost2Dev *read_command = (FisHost2Dev *)self->commands[command_slot].table[0].command_fis;
    *read_command = (FisHost2Dev){};

    read_command->device = 0;
    read_command->command = FIS_CMD_IDENTIFY_DEVICE;
    read_command->port_multiplier = 0;
    read_command->fis_type = FIS_TYPE_REG_HOST2DEV;
    read_command->ctrl_or_cmd = 1;
    ahci_wait_busy_drq(self);

    start_ahci_device_command(self);
    self->port->command_issue = 1 << command_slot;
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
        log$("int-status: {#b}", self->port->interrupt_status);
        log$("sata-status: {#b}", self->port->sata_status);
        log$("cmd-status: {#b}", self->port->command_and_status);
        log$("task-file: {#b}", self->port->task_file_data);
        log$("error: {#b}", self->port->sata_error);
        panic$("task error while {} at: {}", "getting ahci information", command);
    }

    log$("{#b}", self->port->interrupt_status);
    ;
    end_ahci_device_command(self);

    for (int i = 0; i < 255; i++)
    {
        log$("[{}] = {}", i, *(((uint16_t *)self->identify.buf) + i));
    }
    return true;
}

void init_ahci_device(AhciDevice *device, HbaPort *port, int idx)
{
    *device = (AhciDevice){};
    device->idx = idx;
    device->port = port;
    atomic_signal_fence(memory_order_acq_rel);
    atomic_thread_fence(memory_order_acq_rel);
    if (device->port->command_and_status & (HBA_CMD_CMD_LIST_RUNNING | HBA_CMD_START | HBA_CMD_FIS_RECEIVE_RUNNING | HBA_CMD_FIS_RECEIVE_ENABLE))
    {
        log$("setting device to idle");
        device->port->command_and_status &= ~((uint32_t)(HBA_CMD_START | HBA_CMD_FIS_RECEIVE_ENABLE));
        while (device->port->command_and_status & (HBA_CMD_CMD_LIST_RUNNING | HBA_CMD_CMD_LIST_RUNNING))
        {
        }
    }
    port->command_and_status |= 1 << 28;
    atomic_signal_fence(memory_order_acq_rel);
    atomic_thread_fence(memory_order_acq_rel);

    if (port->command_and_status & HBA_CMD_COLD_PRESENCE_STATE)
    {
        log$("cold !");
    }
    atomic_signal_fence(memory_order_acq_rel);
    atomic_thread_fence(memory_order_acq_rel);
    port->command_and_status |= HBA_CMD_SPIN_UP;
    init_ahci_device_command(device);

    // ahci_identify(device, 0xec);
}

bool ahci_io_cmd(AhciDevice *self, BrMemObj target, uint64_t cursor, uint64_t count, bool write)
{
    self->port->interrupt_status = 0xFFFFFFFF;

    int command_slot = ahci_find_command_port(self);
    log$("using slot: {}", command_slot);
    if (command_slot == -1)
    {
        return false;
    }

    HbaCommand *cmd = self->commands[command_slot].command;

    init_hba_command_header(cmd, write, sizeof(FisHost2Dev));
    uintptr_t physical_addr = 0;
    bal_memobj_paddr(target, &physical_addr);
    init_hba_command_table(&self->commands[command_slot], physical_addr, count);

    FisHost2Dev *command_fis = (FisHost2Dev *)self->commands[command_slot].table->command_fis;

    init_fis_reg_command(command_fis, write, cursor, count);

    ahci_wait_busy_drq(self);

    start_ahci_device_command(self);
    atomic_signal_fence(memory_order_acq_rel);
    atomic_thread_fence(memory_order_acq_rel);
    self->port->command_issue |= 1 << command_slot;

    atomic_signal_fence(memory_order_acq_rel);
    atomic_thread_fence(memory_order_acq_rel);
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
        log$("int-status: {#b}", self->port->interrupt_status);
        log$("sata-status: {#b}", self->port->sata_status);
        log$("sata-error: {#b}", self->port->sata_error);
        log$("cmd-status: {#b}", self->port->command_and_status);
        log$("task-file: {#b}", self->port->task_file_data);
        log$("error: {#b}", self->port->sata_error);
        Fis *fis = (Fis *)self->fis_mem.buf;
        log$("error2: {#b}", fis->d2h_fis.error);
        log$("error2: {#b}", fis->d2h_fis.status);
        panic$("task error while {} at: {} count: {}", write ? "writing" : "reading", cursor, count);
    }
    end_ahci_device_command(self);

    return true;
}
