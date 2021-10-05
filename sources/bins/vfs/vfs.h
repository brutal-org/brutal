#pragma once
#include <bal/shared_mem.h>
#include <brutal/ds.h>
#include <brutal/task.h>
#include <proto/fs_dev.h>
#include <proto/vfs.h>
typedef enum
{
    FS_FREE = 0,
    FS_CLOSED,
    FS_OPENED,
    FS_ERR,
} FsStatus;

typedef struct
{
    VfsFileSystemHandle fs_handle;
    FsdevInode fs_inode;

    VfsFileHandle handle;
    Str path;
    FsStatus status;
} VfsFile;

typedef struct
{
    Vec(VfsFile) openned_files;
    TaskId id;
    bool active;
} VfsTask;

VfsError open(SharedMem path, TaskId opener, VfsFileHandle *result);

VfsError write(VfsFileHandle handle, TaskId opener, BrMemObj obj, size_t off, size_t *res);

void vfs_init(void);
