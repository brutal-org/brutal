#include "vfs/vfs.h"
#include <brutal/alloc.h>
#include <brutal/log.h>
#include <brutal/mem.h>
#include "vfs/fs.h"

VfsFileHandle file_next_handle = 1;

Vec(VfsTask) tasks_fd;
void vfs_init(void)
{
    vec_init(&tasks_fd, alloc_global());
}

VfsTask *get_task_fds(TaskId id)
{
    for (int i = 0; i < tasks_fd.length; i++)
    {
        if (tasks_fd.data[i].id == id)
        {
            return tasks_fd.data + i;
        }
    }
    return NULL;
}

VfsTask *get_task_fds_or_create(TaskId id)
{
    VfsTask *r = get_task_fds(id);

    if (r == nullptr)
    {

        VfsTask v = {};
        vec_init(&v.openned_files, alloc_global());

        v.active = true;
        v.id = id;

        vec_push(&tasks_fd, v);
        return get_task_fds(id);
    }

    return r;
}

void add_file(VfsFile *file, VfsTask *self)
{

    for (int i = 0; i < self->openned_files.length; i++)
    {
        if (self->openned_files.data[i].status == FS_FREE || self->openned_files.data[i].status == FS_CLOSED)
        {
            self->openned_files.data[i] = *file;
            return;
        }
    }

    vec_push(&self->openned_files, *file);
}

VfsFile *get_file(VfsFileHandle handle, VfsTask *self)
{
    for (int i = 0; i < self->openned_files.length; i++)
    {
        if (self->openned_files.data[i].handle == handle)
        {
            return self->openned_files.data + i;
        }
    }

    return nullptr;
}

VfsError open(SharedMem path, TaskId opener, VfsFileHandle *result)
{

    Str path_m = shared_mem_to_str(&path);
    VfsFileSystemHandle file_system = get_mounted_fs(path_m);

    if (file_system == 0)
    {
        return VFS_INVALID_PATH;
    }
    auto mount = get_fs(file_system);
    if (mount == nullptr)
    {
        return VFS_INVALID_PATH;
    }

    auto inode = fs_open(path, mount);
    VfsFile file = {
        .handle = file_next_handle++,
        .fs_handle = file_system,
        .fs_inode = inode,
        .path = str_dup(path_m, alloc_global()),
        .status = FS_OPENED,
    };

    VfsTask *t = get_task_fds_or_create(opener);

    add_file(&file, t);

    *result = file.handle;

    return VFS_SUCCESS;
}

VfsError write(VfsFileHandle handle, TaskId opener, BrMemObj obj, size_t off, size_t *res)
{
    VfsTask *t = get_task_fds(opener);

    if (t == nullptr)
    {
        return VFS_INVALID_HANDLE;
    }

    VfsFile *f = get_file(handle, t);

    if (f == nullptr)
    {
        return VFS_INVALID_HANDLE;
    }

    auto mount = get_fs(f->fs_handle);

    *res = fs_write(off, f->fs_inode, obj, mount);

    return VFS_SUCCESS;
}
