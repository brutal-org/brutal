#include <virtio/splitvqueue.h>

typedef struct
{
    VirtioVQueueDesc descriptors[0];
} VirtioSplitVQueue;