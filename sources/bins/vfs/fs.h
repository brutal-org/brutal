#pragma once
#include <bal/shared_mem.h>
#include <proto/fs_dev.h>
#include <proto/vfs.h>
typedef struct
{
    VfsFileSystemHandle self_handle;
    Str mount_point;
    BrHandle task;
} VfsFileSystem;
void vfs_fs_init(void);

VfsFileSystemHandle get_mounted_fs(Str path);

VfsFileSystem *get_fs(VfsFileSystemHandle handle);

VfsFileSystemHandle mount(BrHandle fs_task, SharedMem *path);

BrResult unmount(Str path);

FsdevInode fs_open(SharedMem path, VfsFileSystem *self);

size_t fs_write(size_t off, FsdevInode inode, BrMemObj dat, VfsFileSystem *self);
