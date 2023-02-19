#pragma once

#include <brutal-base>

typedef enum
{
    FIS_TYPE_REG_HOST2DEV = 0x27,
    FIS_TYPE_REG_DEV2HOST = 0x34,
    FIS_TYPE_DMA_ACTIVATE = 0x39,
    FIS_TYPE_DMA_SETUP = 0x41,
    FIS_TYPE_DATA = 0x46,
    FIS_TYPE_BIST = 0x58,
    FIS_TYPE_PIO_SETUP = 0x5F,
    FIS_TYPE_DEV_BITS = 0xA1
} FisTypes;

typedef enum
{
    FIS_CMD_NONE = 0,

    FIS_CMD_PACKET = 0xa0,
    FIS_CMD_DEVICE_RESET = 0x08,
    FIS_CMD_SERVICE = 0xa2,
    FIS_CMD_GET_STATUS = 0xda,

    FIS_CMD_FLUSH = 0xe7,
    FIS_CMD_FLUSH_EXT = 0xea,

    FIS_CMD_DATA_MANAGEMENT = 0x06,
    FIS_CMD_EJECT = 0xed,

    FIS_CMD_IDENTIFY_DEVICE = 0xec,
    FIS_CMD_IDENTIFY_PACKET_DEVICE = 0xa1,

    FIS_CMD_READ_DMA = 0xc8,
    FIS_CMD_READ_DMA_EXT = 0x25,
    FIS_CMD_WRITE_DMA = 0xca,
    FIS_CMD_WRITE_DMA_EXT = 0x35,

    FIS_CMD_READ_DMA_QUEUEUD = 0xc7,
    FIS_CMD_READ_DMA_QUEUED_EXT = 0x26,
    FIS_CMD_WRITE_DMA_QUEUED = 0xcc,
    FIS_CMD_WRITE_DMA_QUEUED_EXT = 0x36,

    FIS_CMD_READ_MULTIPLE = 0xc4,
    FIS_CMD_READ_MULTIPLE_EXT = 0x29,
    FIS_CMD_WRITE_MULTIPLE = 0xc5,
    FIS_CMD_WRITE_MULTPLE_EXT = 0x39,

    FIS_CMD_READ_SECTORS = 0x20,
    FIS_CMD_READ_SECTORS_EXT = 0x24,
    FIS_CMD_WRITE_SECTORS = 0x30,
    FIS_CMD_WRITE_SECTORS_EXT = 0x34,
} FisCommands;

typedef volatile struct PACKED
{
    uint8_t fis_type;
    uint8_t port_multiplier : 4;

    uint8_t _reserved : 3;
    uint8_t ctrl_or_cmd : 1;
    uint8_t command;
    uint8_t feature_1;
    uint8_t lba[3];
    uint8_t device;
    uint8_t lba2[3];
    uint8_t feature_2;

    uint8_t count_l;
    uint8_t count_h;
    uint8_t isochronous_command;
    uint8_t control;
    uint8_t _reserved2[4];
} FisHost2Dev;

typedef volatile struct PACKED
{
    uint8_t fis_type;
    uint8_t port_multiplier : 4;

    uint8_t _reserved : 2;
    uint8_t interrupt : 1;
    uint8_t _reserved1 : 1;
    uint8_t status;
    uint8_t error;
    uint8_t lba[3];
    uint8_t device;
    uint8_t lba2[3];
    uint8_t _reserved2;
    uint8_t countl;
    uint8_t counth;
    uint8_t _reserved3[5];
    // FIFDIDIDI
} FisDev2Host;

typedef volatile struct PACKED
{
    uint8_t fis_type;
    uint8_t port_multiplier : 4;

    uint8_t _reserved : 4;
    uint8_t _reserved1[2];
    uint32_t data[];
} FisData;

typedef volatile struct PACKED
{
    uint8_t fis_type;
    uint8_t port_multiplier : 4;

    uint8_t _reserved : 1;
    uint8_t direction : 1; // 1: dev -> host
    uint8_t interrupt : 1;
    uint8_t _reserved1 : 1;
    uint8_t status;
    uint8_t error;
    uint8_t lba[3];
    uint8_t device;
    uint8_t lba2[3];
    uint8_t _reserved2;
    uint8_t count_l;
    uint8_t count_h;
    uint8_t _reserved3;
    uint8_t e_status;
    uint16_t transfer_count;
    uint8_t _reserved4[2];

} FisPioSetup;

typedef volatile struct PACKED
{
    uint8_t fis_type;
    uint8_t port_multiplier : 4;

    uint8_t _reserved : 1;
    uint8_t direction : 1; // 1: dev->host
    uint8_t interrupt : 1;
    uint8_t auto_activate : 1;
    uint8_t _reserved1[2];
    uint64_t dma_buffer_id;
    uint32_t _reserved2;
    uint32_t dma_buffer_offset;
    uint32_t transfer_count;
    uint32_t _reserved3;
} FisDmaSetup;

typedef volatile struct PACKED
{
    FisDmaSetup dma_setup_fis;
    uint8_t _pad[4];

    FisPioSetup pio_setup_fis;
    uint8_t _pad1[12];

    FisDev2Host d2h_fis;
    uint8_t _pad2[4];

    uint8_t sdbfis[2];
    uint8_t _pad3[64];
    uint8_t _reserved[96];
} Fis;
