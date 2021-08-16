#pragma once

#include <brutal/base.h>
#include <virtio/device.h>
#include <virtio/virtqueue.h>

// All the features of a Virtio Block Device
typedef enum
{
    VIRTIO_BLK_F_SIZE_MAX = 1 << 1,
    VIRTIO_BLK_F_SEG_MAX = 1 << 2,
    VIRTIO_BLK_F_GEOMETRY = 1 << 4,
    VIRTIO_BLK_F_RO = 1 << 5,
    VIRTIO_BLK_F_BLK_SIZE = 1 << 6,
    VIRTIO_BLK_F_FLUSH = 1 << 9,
    VIRTIO_BLK_F_TOPOLOGY = 1 << 10,
    VIRTIO_BLK_F_CONFIG_WCE = 1 << 11,
    VIRTIO_BLK_F_DISCARD = 1 << 13,
    VIRTIO_BLK_F_WRITE_ZEROES = 1 << 14,
} VirtioBlkDeviceFeatures;

// A Virtio Block Device config
typedef struct PACKED
{
    le_uint64_t capacity;
    le_uint32_t size_max;
    le_uint32_t seg_max;

    struct PACKED virtio_blk_geometry
    {
        le_uint16_t cylinders;
        uint8_t heads;
        uint8_t sectors;
    } geometry;

    le_uint32_t block_size;

    struct PACKED virtio_blk_topology
    {
        uint8_t physical_block_exp;
        uint8_t alignment_offset;
        le_uint16_t minimal_io_size;
        le_uint32_t optimal_io_size;
    } topology;

    uint8_t writeback;
    uint8_t unused0[3];
    le_uint32_t max_discard_sectors;
    le_uint32_t max_discard_seg;
    le_uint32_t discard_sector_alignment;
    le_uint32_t max_write_zeroes_sectors;
    le_uint32_t max_write_zeroes_seg;
    uint8_t write_zeroes_way_unmap;
    uint8_t unused1[3];
} VirtioBlockConfig;

typedef struct
{
    VirtioDevice virt_device;
    VirtioBlockConfig device_config;
    // VirtioPackedVirtQ requests_queue;

} VirtioBlockDevice;

VirtioBlockDevice virtio_block_init(VirtioDevice *device, uint64_t needed_features);
