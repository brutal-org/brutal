#include <brutal/debug.h>
#include <fs/block.h>

FsBlockResult fs_block_read(FsBlockImpl *self, void *buf, size_t count, size_t cur)
{
    return self->read((void *)self, buf, count, cur);
}

FsBlockResult fs_block_write(FsBlockImpl *self, void const *buf, size_t count, size_t cur)
{
    assert_not_null(self->write);

    return self->write((void *)self, buf, count, cur);
}
