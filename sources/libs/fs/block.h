#pragma once

#include <bal/hw/mem.h>
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

typedef FsBlockResult FsReadBlock(void *self, void *buf, size_t count, size_t lba);
typedef FsBlockResult FsWriteBlock(void *self, void const *buf, size_t count, size_t lba);

typedef struct
{
    FsReadBlock *read;
    FsWriteBlock *write;
    size_t block_size;
} FsBlockImpl;

FsBlockResult fs_block_read(FsBlockImpl *self, void *buf, size_t count, size_t lba);
FsBlockResult fs_block_write(FsBlockImpl *self, void const *buf, size_t count, size_t lba);
