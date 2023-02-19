#pragma once

#include <virtio/device.h>
#include <virtio/virtqueue.h>
#include <brutal-base>

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

    // Geometry of the block device
    le_uint16_t geometry_cylinders;
    uint8_t geometry_heads;
    uint8_t geometry_sectors;

    le_uint32_t block_size;

    // Topology of the block device
    uint8_t topology_physical_block_exp;
    uint8_t topology_alignment_offset;
    le_uint16_t topology_minimal_io_size;
    le_uint32_t topology_optimal_io_size;

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

// VirtIO block device request types
typedef enum
{
    REQUEST_IN = 0,            // Read
    REQUEST_OUT = 1,           // Write
    REQUEST_FLUSH = 4,         // Flush
    REQUEST_DISCARD = 11,      // Discard
    REQUEST_WRITE_ZEROES = 13, // Write zeroes
} VirtioBlkRequest;

// A VirtIO block device request.
struct PACKED blk_request
{
    le_uint32_t type; // The type of the request. see: VirtioBlkRequest
    le_uint32_t reserved;
    le_uint64_t sector; // The sector to perform request on.
    uint8_t data[512];  // FIXME: the size may vary
    uint8_t status;     // Status of the request.
};

// A VirtIO block device discard that writes zeroes on the discarded sectors
struct PACKED blk_discard_write_zeroes
{
    le_uint64_t sector;      // The first sector the device must write on
    le_uint32_t num_sectors; // The number of sectors the device must write on - 1
    le_uint32_t flags;       // 0: unmap, 31: reservec
};

// The representation of a VirtIO block device.
typedef struct
{
    VirtioDevice virt_device;
    VirtioBlockConfig device_config;
    // VirtioPackedVirtQ requests_queue;
} VirtioBlockDevice;

VirtioBlockDevice virtio_block_init(VirtioDevice *device, uint64_t needed_features);
