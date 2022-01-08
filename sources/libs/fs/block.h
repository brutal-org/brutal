#pragma once

#include <brutal/ds.h>
#include <brutal/io.h>
#include <brutal/mem.h>

typedef enum
{
    FS_BLOCK_SUCCESS,
    FS_BLOCK_ERROR,
    FS_BLOCK_INVALID_LBA,
    FS_BLOCK_INVALID_COUNT,
} FsBlockResult;

typedef enum
{
    FS_BLOCK_READ = 1 << 0,
    FS_BLOCK_WRITE = 1 << 1,
    FS_BLOCK_NOCACHE = 1 << 2,
} FsBlockFlags;

typedef FsBlockResult FsAcquireBlock(void *self, void **buf, size_t count, size_t lba, FsBlockFlags flags);
typedef FsBlockResult FsReleaseBlock(void *self, void const **buf);

typedef struct
{
    FsAcquireBlock *acquire;
    FsReleaseBlock *release;
    size_t block_size;
} FsBlockImpl;

FsBlockResult fs_block_acquire(FsBlockImpl *self, void **buf, size_t count, size_t lba, FsBlockFlags flags);
FsBlockResult fs_block_release(FsBlockImpl *self, void const **buf);
