#pragma once

typedef enum
{
    VIRTIO_F_RING_INDIRECT_DESC = 1 << 28,
    VIRTIO_F_RING_EVENT_IDX = 1 << 29,
    VIRTIO_F_VERSION_1 = 1 << 32,
    VIRTIO_F_ACCESS_PLATFORM = 1 << 33,
    VIRTIO_F_RING_PACKED = 1 << 34, // Indicates the device is compatible with packed virtq
    VIRTIO_F_IN_ORDER = 1 << 35,
    VIRTIO_F_ORDER_PLATFORM = 1 << 36,
    VIRTIO_F_SR_IOV = 1 << 37,
    VIRTIO_F_NOTIFICATION_DATA = 1 << 38
} VirtioGeneralFeatures;