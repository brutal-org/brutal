#include "vfs/fs.h"
#include <ansi/string.h>
#include <bal/shared_mem.h>
#include <brutal/alloc.h>
#include <brutal/ds.h>
#include <brutal/log.h>

VfsFileSystemHandle fs_next_handle = 1;
Vec(VfsFileSystem) devices;

void vfs_fs_init(void)
{
    vec_init(&devices, alloc_global());
}

// FIXME: this function is dumb and we should implement a better one, but for the moment it works.
VfsFileSystemHandle get_mounted_fs(Str path)
{
    VfsFileSystemHandle raw_id = 0;
    size_t cur_path_size = 0;
    vec_foreach(v, &devices)
    {
        if (cur_path_size < v.mount_point.len)
        {
            if (strncmp(path.buffer, v.mount_point.buffer, v.mount_point.len) == 0)
            {
                raw_id = v.self_handle;
            }
        }
    }
    return raw_id;
}

VfsFileSystem *get_fs(VfsFileSystemHandle handle)
{
    for (int i = 0; i < devices.length; i++)
    {
        if (devices.data[i].self_handle == handle)
        {
            return devices.data + i;
        }
    }
    return nullptr;
}

VfsFileSystemHandle mount(BrHandle fs_task, SharedMem *path)
{
    VfsFileSystem dev;
    Str path_str = shared_mem_to_str(path);
    dev.self_handle = fs_next_handle++;
    dev.task = fs_task;
    dev.mount_point = str_dup(path_str, alloc_global());

    vec_push(&devices, dev);
    FsdevMountRequest req =
        {
            .path = path->obj};
    FsdevMountResponse resp = {};
    fsdev_mount(fs_task, &req, &resp);
    log$("mounted fs ({}) at: {}", fs_task, dev.mount_point);

    if (resp.accepted)
    {
        log$("accepted! ");
    }
    else
    {
        log$("error: can't mount");
    }
    return dev.self_handle;
}

BrResult unmount(Str path)
{
    panic_todo$("implement unmount ! ({})", path);
}

FsdevInode fs_open(SharedMem path, VfsFileSystem *self)
{
    FsdevOpenRequest req = {
        .path = path.obj,
    };

    FsdevOpenResponse resp = {};
    fsdev_open(self->task, &req, &resp);

    return resp.handle;
}

size_t fs_write(size_t off, FsdevInode inode, BrMemObj dat, VfsFileSystem *self)
{
    FsdevWriteRequest req = {
        .off = off,
        .handle = inode,
        .data = dat,
    };

    FsdevWriteResponse resp = {};

    fsdev_write(self->task, &req, &resp);

    return resp.written;
}
