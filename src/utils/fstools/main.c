#include <brutal-alloc>
#include <brutal-debug>
#include <brutal-ds>
#include <fs/block.h>
#include <fs/ext2/ext2.h>
/*
    if you want to make a test image: (named here image.hdd)
        - dd if=/dev/zero of=image.hdd bs=512 count=65556
        - mkfs.ext2 -O ^sparse_super,^resize_inode ./image.hdd
*/

typedef struct
{
    void *buf;
    size_t count;
    size_t lba;
    FsBlockFlags flags;
} FileBlockData;

typedef struct
{
    FsBlockImpl _impl;
    IoFile file;
    Alloc *alloc;
    Vec(FileBlockData) blocks;
} FileBlock;

FsBlockResult file_block_acquire(FileBlock *self, void **buf, size_t count, size_t lba, FsBlockFlags flags)
{
    FileBlockData block = {
        .flags = flags,
        .lba = lba,
        .count = count,
    };

    block.buf = alloc_malloc(self->alloc, count * 512);

    if (flags & FS_BLOCK_READ)
    {
        io_pread(io_file_rseek(&self->file), block.buf, count * 512, lba * 512);
    }

    *buf = block.buf;

    vec_push(&self->blocks, block);

    return OK(FsBlockResult, (Success){});
}

FsBlockResult file_block_release(FileBlock *self, void const **buf)
{
    FileBlockData *block = nullptr;
    long block_idx = 0;

    vec_foreach_idx(i, v, &self->blocks)
    {
        if (v->buf == buf)
        {
            block = v;
            block_idx = i;
        }
    }

    if (block == nullptr)
    {
        return ERR(FsBlockResult, FS_BLOCK_ERROR);
    }

    if (block->flags & FS_BLOCK_WRITE)
    {
        io_pwrite(io_file_wseek(&self->file), block->buf, block->count * 512, block->lba * 512);
    }

    vec_splice(&self->blocks, block_idx, 1);

    alloc_free(self->alloc, block->buf);

    return OK(FsBlockResult, (Success){});
}

Emit emit;
Iter file_block_dump(Ext2FsFile *file, void *ctx)
{
    emit_ident(&emit);
    emit_fmt$(&emit, "- founded file: {}\n", file->name);
    if (!str_eq(file->name, str$(".")) && !str_eq(file->name, str$("..")))
    {
        ext2_fs_iter(ctx, &file->inode, (Ext2IterFileFn *)file_block_dump, ctx);
    }
    emit_deident(&emit);
    return ITER_CONTINUE;
}

int main(int argc, char const *argv[])
{
    if (argc == 1)
    {
        log$("usage: {} [disk]", argv[0]);
        return 0;
    }

    IoFile source_file;
    UNWRAP_OR_PANIC(io_file_edit(&source_file, str$(argv[1])), "File not found!");

    FileBlock block = {
        ._impl = {
            .acquire = (FsAcquireBlock *)file_block_acquire,
            .release = (FsReleaseBlock *)file_block_release,
            .block_size = 512,
        },
        .file = source_file,
        .alloc = alloc_global(),
    };

    vec_init(&block.blocks, alloc_global());

    emit_init(&emit, io_chan_out());
    Ext2Fs fs = {};
    UNWRAP(ext2_init(&fs, &block._impl, alloc_global()));
    Ext2FsInode root_inode = {};

    UNWRAP(ext2_inode(&fs, &root_inode, 2));

    ext2_fs_iter(&fs, &root_inode, (Ext2IterFileFn *)file_block_dump, &fs);

    io_file_close(&source_file);
    return 0;
}
