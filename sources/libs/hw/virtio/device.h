#pragma once

#include <brutal-base>

#define VIRTIO_STATUS_ACKNOWLEDGE (1 << 0)      // Indicates that the guest OS has found the device and recognized it as a valid virtio device.
#define VIRTIO_STATUS_DRIVER_AVAILABLE (1 << 1) // Indicates that the guest OS knows how to drive the device.
#define VIRTIO_STATUS_FAILURE (1 << 7)          // Indicates that something went wrong in the guest, and it has given up on the device.
#define VIRTIO_STATUS_FEATURES_OK (1 << 3)      // Indicates that the driver has acknowledged all the features it understands, and feature negotiation is complete.
#define VIRTIO_STATUS_DRIVER_OK (1 << 2)        // Indicates that the driver is set up and ready to drive the device.
#define VIRTIO_STATUS_NEEDS_RESET (1 << 6)      // Indicates that the device has experienced an error from which it can’t recover.

// Represents a VirtIO device
typedef struct PACKED
{
    uint8_t device_status;
    uint64_t device_features;
} VirtioDevice;

// Values returned for each operation made on `VirtioDevice`
typedef enum
{
    VIRTIO_DEVICE_OK,

    VIRTIO_DEVICE_UNKNOWN_STATUS,
    VIRTIO_DEVICE_FEATURES_NEGOCIATE_FAILED,
} VirtioDeviceResult;

// TODO: VARIANT SIZE TO DEFINE
typedef enum
{
    VIRTIO_INVALID,
    VIRTIO_NETWORK_CARD,
    VIRTIO_BLOCK,
    VIRTIO_CONSOLE,
    VIRTIO_ENTROPY,
    VIRTIO_MEM_BALOONING,
    VIRTIO_IO_MEMORY,
    VIRTIO_RPMSG,
    VIRTIO_SCSI_HOST,
    VIRTIO_9P_TRANSPORT,
    VIRTIO_MAC80211_WLAN,
    VIRTIO_RPROC_SERIAL,
    VIRTIO_CAIF,
    VIRTIO_MEMORY_BALLOON,
    VIRTIO_GPU,
    VIRTIO_TIMER_CLOCK,
    VIRTIO_INPUT,
    VIRTIO_SOCKET,
    VIRTIO_CRYPTO,
    VIRTIO_SIGNAL_DISTRIB,
    VIRTIO_PSTORE,
    VIRTIO_IOMMU,
    VIRTIO_MEMORY,
} VirtioDeviceType;

// A type representing a virtio device class. This must be passed as a parameter by the device-specific handler
typedef struct
{
    VirtioDeviceType device_type;
    uint64_t needed_features;
} VirtioDeviceInit;

typedef uint64_t VirtioDriverFeatures;

// Returns true if `status` is set, false otherwise
bool virtio_in_status(VirtioDevice *device, uint8_t status);

// Sets the `status` on the virtio device
VirtioDeviceResult virtio_set_status(VirtioDevice *device, uint8_t status);

// Initializes the virtio device at `base_address`
// This function doesn't setup the device-specific needings
// After this the `device` must be set in VIRTIO_STATUS_DRIVER_OK
VirtioDeviceResult virtio_device_init(VirtioDevice *device, VirtioDeviceInit *device_class);

// Negociates features with the virtio device for the caller.
// Returns VIRTIO_DEVICE_FEATURES_NEGOCIATE_FAILED the driver cannot negociate any feature with the virtio device
VirtioDeviceResult virtio_device_negociate_features(VirtioDevice *device, VirtioDeviceInit *init);
