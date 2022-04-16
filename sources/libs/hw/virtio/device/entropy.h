#pragma once

#include <virtio/device.h>
#include <virtio/virtqueue.h>
#include <brutal-base>

typedef struct
{
    VirtioDevice virt_device;

    // virtioqueue requests_queue
} VirtioEntropyDevice;

VirtioEntropyDevice virtio_entropy_init(VirtioDevice *device);
