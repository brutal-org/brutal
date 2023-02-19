#include <fs/block.h>
#include <brutal-debug>

FsBlockResult fs_block_acquire(FsBlockImpl *self, void **buf, size_t count, size_t lba, FsBlockFlags flags)
{
    assert_not_null(self->acquire);

    return self->acquire((void *)self, buf, count, lba, flags);
}

FsBlockResult fs_block_release(FsBlockImpl *self, void const **buf)
{
    assert_not_null(self->release);

    return self->release((void *)self, buf);
}
