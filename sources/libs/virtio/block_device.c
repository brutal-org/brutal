#include <virtio/block_device.h>
#include <virtio/device.h>

VirtioDeviceResult block_setup(VirtioDevice *device)
{
    // TODO: VirtIO block device initialization...
    return VIRTIO_DEVICE_OK;
}

VirtioBlockDevice virtio_block_init(VirtioDevice *device, uint64_t needed_features)
{
    VirtioDeviceInit init = {
        .device_type = VIRTIO_BLOCK,
        .needed_features = needed_features,
        .setup_func = (SetupFunc)block_setup,
    };

    return (VirtioBlockDevice){};
}
