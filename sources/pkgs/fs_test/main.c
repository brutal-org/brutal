#include <brutal/alloc.h>
#include <brutal/debug.h>
#include <fs/block.h>
#include <fs/ext2/ext2.h>

/*
    this file is only for testing, it will be deleted later on, don't expect good code here
    if you want to make a test image: (named here ext2disk)
        - dd if=/dev/zero of=ext2disk.hdd bs=512 count=65556
        - mkfs.ext2 -O ^sparse_super,^resize_inode ./ext2disk.hdd
*/

typedef struct
{
    FsBlockImpl _impl;
    IoFile file;
} FileBlock;

FsBlockResult file_block_write(FileBlock *self, const void *buf, size_t count, size_t lba)
{
    if (!embed_file_seek(&self->file.embed, lba * 512, SEEK_SET).succ)
    {
        return FS_BLOCK_INVALID_LBA;
    }
    if (!embed_file_write(&self->file.embed, buf, count * 512).succ)
    {
        return FS_BLOCK_ERROR;
    }

    return FS_BLOCK_SUCCESS;
}

FsBlockResult file_block_read(FileBlock *self, void *buf, size_t count, size_t lba)
{
    if (!embed_file_seek(&self->file.embed, lba * 512, SEEK_SET).succ)
    {
        return FS_BLOCK_INVALID_LBA;
    }
    if (!embed_file_read(&self->file.embed, buf, count * 512).succ)
    {
        return FS_BLOCK_ERROR;
    }

    return FS_BLOCK_SUCCESS;
}

Emit emit;
Iter file_block_dump(Ext2FsInode *inode, void *ctx)
{
    emit_ident(&emit);
    emit_fmt(&emit, "- founded inode: {}\n", inode->name);
    if (!str_eq(inode->name, str$(".")) && !str_eq(inode->name, str$("..")))
    {
        //   ext2_fs_dump_inode(ctx, inode);
        ext2_fs_iter(ctx, inode, (IterFn *)file_block_dump, ctx);
    }
    emit_deident(&emit);
    return ITER_CONTINUE;
}

int main(MAYBE_UNUSED int argc, MAYBE_UNUSED char const *argv[])
{

    log$("started fs_test app");
    if (argc == 1)
    {
        log$("usage: fs_test [disk]");
        return 0;
    }

    Str path = str$(argv[1]);

    IoFile source_file;
    UNWRAP_OR_PANIC(io_file_open(&source_file, path), "File not found!");

    FileBlock block = {
        ._impl = {
            .read = (FsReadBlock *)file_block_read,
            .write = (FsWriteBlock *)file_block_write,
            .block_size = 512,
        },
        .file = source_file,
    };

    emit_init(&emit, io_chan_out());
    Ext2Fs fs = {};

    ext2_init(&fs, &block._impl, alloc_global());
    Ext2FsInode root_inode = {};

    ext2_inode(&fs, &root_inode, 2);

    ext2_fs_iter(&fs, &root_inode, (IterFn *)file_block_dump, &fs);
    return 0;
}
