#pragma once
#include <fs/block.h>
#include <fs/ext2/structure.h>
#include <fs/fs.h>

typedef uint64_t Ext2FsInodeId;
typedef struct
{
    FsBlockImpl *block_device;
    Ext2SuperBlock *super_block;
    size_t block_size;
    uint32_t group_desc_table_block;
    uint32_t group_count;
    Alloc *alloc;
} Ext2Fs;

typedef struct
{
    uint32_t id;
    Ext2InodeBlock block;
    Str name; /* we can only have the name in the parent directory, so sometimes, this field may be empty */
} Ext2FsInode;

typedef Range(long) BlockRange;

FsResult ext2_init(Ext2Fs *self, FsBlockImpl *block, Alloc *alloc);

Iter ext2_fs_iter(Ext2Fs *self, Ext2FsInode *inode, IterFn fn, void *ctx);

FsResult ext2_inode(Ext2Fs *self, Ext2FsInode *inode, Ext2FsInodeId id);
FsResult ext2_inode_read(Ext2Fs *self, Ext2FsInode *inode, void *data, BlockRange range);

void ext2_fs_dump_inode(Ext2Fs *self, Ext2FsInode *inode);
