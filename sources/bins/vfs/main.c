#include <bal/shared_mem.h>
#include <brutal/log.h>
#include "vfs/fs.h"
#include "vfs/vfs.h"
VfsServer server;

VfsError vfs_server_mount(MAYBE_UNUSED BrId from, VfsMountRequest const *req, VfsMountResponse *res, MAYBE_UNUSED void *ctx)
{
    SharedMem mem = memobj_to_shared(req->path);
    assert_br_success(shared_mem_map(&mem));
    res->devid = mount(req->fshandle, &mem);

    shared_mem_free(&mem);
    return VFS_SUCCESS;
}

VfsError vfs_server_unmount(MAYBE_UNUSED BrId from, MAYBE_UNUSED VfsUmountRequest const *req, VfsUmountResponse *res, MAYBE_UNUSED void *ctx)
{
    from = (uintptr_t)ctx;
    res->devid = 0;
    return VFS_SUCCESS;
}

VfsError vfs_server_open(BrId from, VfsOpenRequest const *req, VfsOpenResponse *res, MAYBE_UNUSED void *ctx)
{
    log$("vfs openning: {}", req->path);
    SharedMem mem = memobj_to_shared(req->path);
    assert_br_success(shared_mem_map(&mem));

    VfsError err = open(mem, from, &res->handle);

    shared_mem_free(&mem);
    return err;
}

VfsError vfs_server_write(BrId from, VfsWriteRequest const *req, VfsWriteResponse *res, MAYBE_UNUSED void *ctx)
{
    SharedMem mem = memobj_to_shared(req->data);
    assert_br_success(shared_mem_map(&mem));

    return write(req->handle, from, req->data, req->off, &res->written);
}

void br_event(BrMsg const *msg)
{
    vfs_server_dispatch(&server, msg);
}

void vfs_error_fn(BrId from, BrResult error, MAYBE_UNUSED void *context)
{
    log$("vfs error: {} (from: {})", br_result_to_string(error), from);
}

int br_entry_args(
    MAYBE_UNUSED long arg1,
    MAYBE_UNUSED long arg2,
    MAYBE_UNUSED long arg3,
    MAYBE_UNUSED long arg4,
    MAYBE_UNUSED long arg5)
{
    server.handle_error = vfs_error_fn;
    server.handle_close = NULL;
    server.handle_open = vfs_server_open;
    server.handle_write = vfs_server_write;
    server.handle_mount = vfs_server_mount;
    server.handle_umount = vfs_server_unmount;

    vfs_fs_init();
    vfs_init();
    return br_ev_run();
}
