#pragma once

#include <brutal/base.h>
#include <virtio/device.h>
#include <virtio/virtqueue.h>

typedef struct
{
    VirtioDevice virt_device;

    // virtioqueue requests_queue
} VirtioEntropyDevice;

VirtioEntropyDevice virtio_entropy_init(VirtioDevice *device);