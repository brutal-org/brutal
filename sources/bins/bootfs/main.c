#include <bal/helpers.h>
#include <bal/shared_mem.h>
#include <brutal/base.h>
#include <brutal/log.h>
#include <brutal/task.h>
#include <proto/fs_dev.h>
#include <proto/vfs.h>

FsdevServer server = {0};

FsdevError bootfs_open(MAYBE_UNUSED BrId from, FsdevOpenRequest const *req, FsdevOpenResponse *resp, MAYBE_UNUSED void *context)
{
    SharedMem mem = memobj_to_shared(req->path);
    assert_br_success(shared_mem_map(&mem));

    Str path = shared_mem_to_str(&mem);
    log$("openning file: {}", path);

    shared_mem_free(&mem);
    resp->handle = 1001;

    return FSDEV_SUCCESS;
}

FsdevError bootfs_write(MAYBE_UNUSED BrId from, FsdevWriteRequest const *req, FsdevWriteResponse *resp, MAYBE_UNUSED void *context)
{

    if (req->handle != 1001)
    {
        return FSDEV_INVALID_PATH;
    }

    log$("tryied to write to file:");
    resp->written = 0;

    SharedMem mem = memobj_to_shared(req->data);

    shared_mem_map(&mem);

    Str dat = shared_mem_to_str(&mem);

    log$("- data: {}", dat);

    shared_mem_free(&mem);
    return FSDEV_SUCCESS;
}
void br_event(BrMsg const *msg)
{
    fsdev_server_dispatch(&server, msg);
}
void bfs_error_fn(BrId from, BrResult error, MAYBE_UNUSED void *context)
{
    log$("bfs error: {} (from: {})", br_result_to_string(error), from);
}

FsdevError bfs_mounted(BrId from, FsdevMountRequest const *req, FsdevMountResponse *resp, MAYBE_UNUSED void *context)
{
    SharedMem mem = memobj_to_shared(req->path);
    assert_br_success(shared_mem_map(&mem));
    Str dat = shared_mem_to_str(&mem);

    log$("mounting boot fs: {} by {}", dat, from);
    resp->accepted = true;

    shared_mem_free(&mem);

    return FSDEV_SUCCESS;
}
int br_entry_args(
    MAYBE_UNUSED long arg1,
    MAYBE_UNUSED long arg2,
    MAYBE_UNUSED long arg3,
    MAYBE_UNUSED long arg4,
    MAYBE_UNUSED long arg5)
{

    server.handle_error = bfs_error_fn;
    server.handle_open = bootfs_open;
    server.handle_write = bootfs_write;
    server.handle_mount = bfs_mounted;
    return br_ev_run();
}
