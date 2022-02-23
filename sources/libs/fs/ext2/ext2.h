#pragma once
#include <fs/block.h>
#include <fs/ext2/structure.h>
#include <fs/fs.h>

typedef uint64_t Ext2FsInodeId;

typedef Vec(uint32_t) Ext2Blocks;

typedef struct
{
    uint32_t id;
    Ext2InodeBlock block;
} Ext2FsInode;

typedef struct
{
    Str name;
    Ext2FsInode inode;
} Ext2FsFile;

typedef struct
{
    Ext2Blocks block;
    Ext2FsInode inode;
} Ext2CachedInode;

typedef struct
{
    FsBlockImpl *block_device;
    Ext2SuperBlock *super_block;
    size_t block_size;
    uint32_t group_desc_table_block;
    uint32_t group_count;
    Vec(Ext2CachedInode) inode_cache;
    Alloc *alloc;
} Ext2Fs;

typedef Range(long) BlockRange;

typedef Result(FsError, Success) FsResult;

FsResult ext2_init(Ext2Fs *self, FsBlockImpl *block, Alloc *alloc);

typedef Iter Ext2IterFileFn(Ext2FsFile *val, void *ctx);

Iter ext2_fs_iter(Ext2Fs *self, Ext2FsInode *inode, Ext2IterFileFn fn, void *ctx);

FsResult ext2_inode(Ext2Fs *self, Ext2FsInode *inode, Ext2FsInodeId id);

FsResult ext2_inode_read(Ext2Fs *self, const Ext2FsInode *inode, void *data, BlockRange range);

FsResult ext2_inode_write(Ext2Fs *self, Ext2FsInode *inode, const void *data, BlockRange range);

void ext2_fs_dump_inode(Ext2Fs *self, Ext2FsInode *inode);

FsResult ext2_group_read(Ext2Fs *self, Ext2GroupBlockDescTable *group, uint64_t group_id);

FsResult ext2_group_write(Ext2Fs *self, Ext2GroupBlockDescTable *group, uint64_t group_id);
