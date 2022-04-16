#pragma once

#include <brutal-base>

typedef enum
{
    HBA_SIG_ATA = 0x101,
    HBA_SIG_ATAPI = 0xEB140101,
    HBA_SIG_SEMB = 0xC33C0101,
    HBA_SIG_PM = 0x96690101,
    HBA_SIG_UNCONNECTED = 0xFFFFFFFF
} HbaPortSig;

typedef enum
{
    HBA_PORT_IPM_ACTIVE = 0b1,
    HBA_PORT_DET_PRESENT = 0b11,
} HbaPortStatus;

// see: AHCI rev 1.3.1: 3.3.7

typedef enum
{
    HBA_CMD_START = 1 << 0,
    HBA_CMD_SPIN_UP = 1 << 1,
    HBA_CMD_POWER_ON = 1 << 2,
    HBA_CMD_CMD_LIST_OVERRIDE = 1 << 3,
    HBA_CMD_FIS_RECEIVE_ENABLE = 1 << 4,
    HBA_CMD_MECHANICAL_SWITCH_STATE = 1 << 13,
    HBA_CMD_FIS_RECEIVE_RUNNING = 1 << 14,
    HBA_CMD_CMD_LIST_RUNNING = 1 << 15,
    HBA_CMD_COLD_PRESENCE_STATE = 1 << 16,
    HBA_CMD_PORT_MULTIPLIER = 1 << 17,
    HBA_CMD_HOTPLUG_SUPPORT = 1 << 18,
    HBA_CMD_MECH_PRESENCE_SWITCH_TO_PORT = 1 << 19,
    HBA_CMD_COLD_PRESENCE_DETECTION = 1 << 20,
    HBA_CMD_EXT_SATA_PORT = 1 << 21,
    HBA_CMD_FIS_SWITCH_CAPABLE = 1 << 22,
    HBA_CMD_IS_ATAPI = 1 << 24,
    HBA_CMD_ENABLE_DRIVE_LED = 1 << 25,
    HBA_CMD_AGGRESSIVE_LINK_ENABLE = 1 << 26,
    HBA_CMD_AGGRSSIVE_SLUMBER = 1 << 27,
    HBA_CMD_ICC_SET_ACTIVE = 1 << 28,
    HBA_CMD_ICC_SET_PARTIAL = 2 << 28,
    HBA_CMD_ICC_SET_SLUMBER = 6 << 28,
    HBA_CMD_ICC_SET_SLEEP = 8 << 28,
} HbaPortCommandStatus;

typedef enum
{
    HBA_DEVICE_BUSY = 0x80,
    HBA_DEVICE_DRQ = 0x08,
} HbaTaskFileDataStatus;

typedef enum
{
    HBA_INT_D2H_INTERRUPT = 1 << 0,
    HBA_INT_PIO_SETUP_INTERRUPT = 1 << 1,
    HBA_INT_DMA_SETUP_INTERRUPT = 1 << 2,
    HBA_INT_SET_BITS_INTERRUPT = 1 << 3,
    HBA_INT_UNKNOWN_INTERRUPT = 1 << 4,
    HBA_INT_DESCRIPTOR_PROCESSED = 1 << 5,
    HBA_INT_PORT_CONNECT_CHANGED_STATUS = 1 << 6,
    HBA_INT_DEV_MECHANICAL_PRESENCE_STATUS = 1 << 7,
    HBA_INT_TASK_FILE_ERROR_STATUS = 1 << 30,
} HbaInterruptStatus;

typedef enum
{
    HBA_SCONTROL_RESET = 1 << 0,
    HBA_SCONTROL_DISABLE = 1 << 2,
    HBA_SCONTROL_RESTRICT_SPEED_GEN1 = 1 << 3,
    HBA_SCONTROL_RESTRICT_SPEED_GEN2 = 2 << 3,
    HBA_SCONTROL_RESTRICT_SPEED_GEN3 = 3 << 3,
} HbaSataControls;

typedef volatile struct PACKED
{
    uint8_t command_len : 5; // multiplied by 4 byte
    uint8_t atapi : 1;
    uint8_t write : 1;
    uint8_t prefetchable : 1;

    uint8_t reset : 1;
    uint8_t bist : 1;
    uint8_t clear_on_ok : 1;
    uint8_t _reserved : 1;
    uint8_t port_multiplier : 4;
    uint16_t physical_region_descriptor_count;
    uint32_t physical_region_decriptor_byte_transferred;

    uint32_t command_table_descriptor_low;
    uint32_t command_table_descriptor_up;

    uint32_t _reserved1[4];
} HbaCommand;

typedef volatile struct PACKED
{
    uint32_t command_list_addr_low;
    uint32_t command_list_addr_up;

    uint32_t fis_addr_low;
    uint32_t fis_addr_up;

    uint32_t interrupt_status;
    uint32_t interrupt_enable;

    uint32_t command_and_status;

    uint32_t _reserved;

    uint32_t task_file_data;

    uint32_t signature;

    uint32_t sata_status;
    uint32_t sata_control;
    uint32_t sata_error;
    uint32_t sata_active;

    uint32_t command_issue;

    uint32_t sata_notification;

    uint32_t fis_based_switch_control;

    uint32_t _reserved_1[11];
    uint32_t vendor[4];
} HbaPort;

typedef volatile struct PACKED
{
    uint32_t capability;

    volatile uint32_t global_host_control;
    uint32_t interrupt_status;
    uint32_t port_implemented;

    uint32_t version;

    uint32_t command_completion_coalescin_control;
    uint32_t command_completion_coalescin_ports;

    uint32_t enclosure_management_location;
    uint32_t enclosure_management_control;

    uint32_t capability_extended;
    uint32_t bios_handoff_control_status;

    uint8_t _reserved[52];
    uint8_t nvmhci[64];
    uint8_t vendor[96];

    HbaPort ports[32];
} HbaMemory;

typedef volatile struct PACKED
{
    uint32_t data_base_low;
    uint32_t data_base_up;
    uint32_t _reserved;
    uint32_t dbc : 22;
    uint16_t _reserved1 : 8;
    uint8_t interrupt_on_complete : 1;
} HbaPrdtEntry;
typedef volatile struct PACKED
{
    uint8_t command_fis[64];
    uint8_t atapi_command[16];
    uint8_t _reserved[48];
    HbaPrdtEntry entry[];
} HbaCommandTable;
