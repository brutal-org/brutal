#include <virtio/device.h>
#include <virtio/device/block.h>
#include <virtio/virtqueue.h>

VirtioDeviceResult block_setup(VirtioDevice *device)
{
    // TODO: VirtIO block device initialization...
    return VIRTIO_DEVICE_OK;
}

VirtioBlockDevice virtio_block_init(VirtioDevice *device, uint64_t needed_features)
{
    VirtioDeviceInit device_init = {
        .device_type = VIRTIO_BLOCK,
        .needed_features = needed_features,
    };

    virtio_device_init(device, &device_init);

    return (VirtioBlockDevice){};
}
