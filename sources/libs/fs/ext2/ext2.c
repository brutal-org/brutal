#include <brutal/base.h>
#include <brutal/debug.h>
#include <fs/ext2/ext2.h>
#include <math.h>

FsResult ext2_read_blocks(Ext2Fs *self, void *data, size_t off, size_t count)
{
    size_t block_diff = self->block_size / self->block_device->block_size;
    if (fs_block_read(self->block_device, data, count * block_diff, off * block_diff) != FS_BLOCK_SUCCESS)
    {
        return FS_ERROR_FROM_BLOCK;
    }
    return FS_SUCCESS;
}

FsResult ext2_write_blocks(Ext2Fs *self, void *data, size_t off, size_t count)
{
    size_t block_diff = self->block_size / self->block_device->block_size;
    if (fs_block_read(self->block_device, data, count * block_diff, off * block_diff) != FS_BLOCK_SUCCESS)
    {
        return FS_ERROR_FROM_BLOCK;
    }
    return FS_SUCCESS;
}

static FsResult ext2_get_group(Ext2Fs *self, Ext2GroupBlockDescTable *group, uint64_t group_id)
{
    size_t block_off = group_id % self->block_size;
    size_t block_id = group_id / self->block_size + self->group_desc_table_block;

    Ext2GroupBlockDescTable *desc = alloc_malloc(self->alloc, self->block_size);
    FsResult res = ext2_read_blocks(self, desc, block_id, 1);
    if (res != FS_SUCCESS)
    {
        return res;
    }

    *group = desc[block_off];

    return FS_SUCCESS;
}

static size_t get_entry_in_indirect_block(size_t entry_per_block, int depth)
{
    size_t res = 1;
    for (int i = 0; i < depth; i++)
    {
        res *= entry_per_block;
    }
    return res;
}

static uint64_t get_inode_size(Ext2SuperBlock const *super_block, Ext2FsInode const *inode)
{
    uint64_t len = inode->block.size_low;
    if (super_block->read_only_feature & EXT2_FEAT_64BIT_FILE_SIZE)
    {
        len |= (uint64_t)inode->block.size_upper << 32;
    }

    return len;
}

// this function read recursively every block
// here is an example if each block was containing 2 sub blocks
// the depth represent the distance we have from blocks containing the data
// you can se it as this:
// [-0--------][-1--------] depth 2
// [-0--][-1--][-2--][-3--] depth 1
// [0][1][2][3][4][5][6][7] depth 0

static FsResult ext2_inode_read_indirect_block_rec(Ext2Fs *self, void **data_cur, BlockRange range, size_t depth, uint32_t indirect_ptr)
{
    // we need to read the block when the depth is 0
    if (depth == 0)
    {
        *data_cur += self->block_size;
        return ext2_read_blocks(self, *data_cur, range.base, 1);
    }

    size_t entry_per_block = self->block_size / sizeof(uint32_t);
    size_t entry_in_indirect_rec = get_entry_in_indirect_block(entry_per_block, depth);
    size_t block_per_entry_rec = entry_in_indirect_rec / entry_per_block;

    uint32_t *indirect_table = alloc_malloc(self->alloc, self->block_size);

    FsResult res = ext2_read_blocks(self, indirect_table, indirect_ptr, 1);
    if (res != FS_SUCCESS)
    {
        return res;
    }

    for (size_t block = range.base; block < MIN((size_t)range_end(range), entry_in_indirect_rec); block += block_per_entry_rec)
    {
        size_t table_id = block / block_per_entry_rec;
        size_t entry_in_table = block % block_per_entry_rec;
        BlockRange sub_range = range_from_start_and_end(BlockRange, entry_in_table, range_end(range));

        res = ext2_inode_read_indirect_block_rec(self, data_cur, sub_range, depth - 1, indirect_table[table_id]);
        if (res != FS_SUCCESS)
        {
            return res;
        }
    }

    alloc_free(self->alloc, indirect_table);

    return FS_SUCCESS;
}

FsResult ext2_inode_read(Ext2Fs *self, Ext2FsInode *inode, void *data, BlockRange range)
{
    // single block
    long block;
    FsResult res;
    for (block = range.base; block < MIN(range_end(range), 12); block++)
    {
        res = ext2_read_blocks(self, data, inode->block.blocks_ptr[block], 1);
        if (res != FS_SUCCESS)
        {
            return res;
        }

        data += self->block_size;
    }
    range = range_from_start_and_end(BlockRange, block - 12, range_end(range) - 12);
    if (range_end(range) < 0)
    {
        return FS_SUCCESS;
    }

    // single indirect block
    res = ext2_inode_read_indirect_block_rec(self, &data, range, 1, inode->block.single_indirect_block_ptr);
    if (res != FS_SUCCESS)
    {
        return res;
    }
    range = range_from_start_and_end(BlockRange, range.base - self->block_size, range_end(range) - self->block_size);

    if (range_end(range) < 0)
    {
        return FS_SUCCESS;
    }
    // double indirect block

    res = ext2_inode_read_indirect_block_rec(self, &data, range, 2, inode->block.double_indirect_block_ptr);
    if (res != FS_SUCCESS)
    {
        return res;
    }

    range = range_from_start_and_end(BlockRange, range.base - self->block_size * self->block_size, range_end(range) - self->block_size * self->block_size);
    if (range_end(range) < 0)
    {
        return FS_SUCCESS;
    }
    // triple indirect block
    res = ext2_inode_read_indirect_block_rec(self, &data, range, 3, inode->block.triple_indirect_block_ptr);
    return res;
}

FsResult ext2_init(Ext2Fs *self, FsBlockImpl *block, Alloc *alloc)
{
    assert_equal((size_t)(block->block_size % 512), (size_t)0);
    *self = (Ext2Fs){};
    self->block_device = block;
    self->alloc = alloc;

    self->super_block = alloc_malloc(alloc, EXT2_SUPER_BLOCK_SIZE);

    if (fs_block_read(block, self->super_block, EXT2_SUPER_BLOCK_BASE / block->block_size, EXT2_SUPER_BLOCK_SIZE / block->block_size) != FS_BLOCK_SUCCESS)
    {
        return FS_ERROR_FROM_BLOCK;
    }

    if (self->super_block->signature != EXT2_SIGNATURE)
    {
        log$("invalid ext2 fs signature");
        return FS_INVALID_FS;
    }

    self->block_size = 1 << (self->super_block->block_size + 10);

    if (self->block_size < self->block_device->block_size)
    {
        log$("not supported: fs device with higher block size than ext2fs");
        return FS_INVALID_FS;
    }

    ext2_dump_superblock(self->super_block);

    self->group_desc_table_block = self->block_size == 1024 ? 2 : 1;

    self->group_count = self->super_block->inode_count / self->super_block->group_inode_count;

    return FS_SUCCESS;
}

FsResult ext2_inode(Ext2Fs *self, Ext2FsInode *inode, Ext2FsInodeId id)
{
    if (id <= 1)
    {
        return FS_INVALID_INODE;
    }
    size_t inode_id = (id - 1) % self->super_block->group_inode_count;
    size_t group_id = (id - 1) / self->super_block->group_inode_count;
    size_t inode_off_in_block = (inode_id * self->super_block->inode_size) % self->block_size;
    size_t inode_block = (inode_id * self->super_block->inode_size) / self->block_size;

    Ext2GroupBlockDescTable group_desc = {};
    FsResult res = ext2_get_group(self, &group_desc, group_id);
    if (res != FS_SUCCESS)
    {
        return res;
    }

    uint8_t *inode_block_data = alloc_malloc(self->alloc, self->block_size);

    res = ext2_read_blocks(self, inode_block_data,
                           group_desc.inode_table_addr + inode_block,
                           1);
    if (res != FS_SUCCESS)
    {
        return res;
    }

    // we can't use Ext2InodeBlock as an array as inode_size can be != sizeof(Ext2InodeBlock)
    inode->block = *(Ext2InodeBlock *)(inode_block_data + inode_off_in_block);
    inode->id = id;

    return FS_SUCCESS;
}

Iter ext2_fs_iter(Ext2Fs *self, Ext2FsInode *inode, IterFn fn, void *ctx)
{
    if (!((inode->block.mode >> 12) == EXT2_INODE_DIRECTORY))
    {
        return ITER_CONTINUE;
    }

    uint64_t size = get_inode_size(self->super_block, inode);
    void *block_data = alloc_malloc(self->alloc, self->block_size);
    for (uint64_t i = 0; i < size; i += self->block_size)
    {
        ext2_inode_read(self, inode, block_data, (BlockRange){.base = i / self->block_size, .size = 1});
        Ext2Directory *dir = block_data;
        while (dir->type != EXT2_FT_UNKOWN && (uintptr_t)dir - (uintptr_t)block_data < self->block_size)
        {
            Ext2FsInode sub_inode;
            ext2_inode(self, &sub_inode, dir->inode);
            sub_inode.name = str_n$(dir->name_len, (char *)dir->name_chars);

            if (fn(&sub_inode, ctx) == ITER_STOP)
            {
                alloc_free(self->alloc, block_data);
                return ITER_STOP;
            }

            dir = (Ext2Directory *)((uint64_t)dir + dir->size);
        }
    }

    alloc_free(self->alloc, block_data);
    return ITER_CONTINUE;
}

void ext2_fs_dump_inode(Ext2Fs *self, Ext2FsInode *inode)
{
    log$("Inode dump: {}", inode->id);

    ext2_dump_inodeblock(&inode->block);

    uint64_t size = get_inode_size(self->super_block, inode);
    void *block = alloc_malloc(self->alloc, self->block_size);

    if ((inode->block.mode >> 12) != EXT2_INODE_DIRECTORY)
    {
        for (uint64_t i = 0; i < size; i += self->block_size)
        {
            ext2_inode_read(self, inode, block, (BlockRange){.base = i / self->block_size, .size = 1});
            log$(" - content: [{}]: '{}'", i, (str_n$(self->block_size, (char *)block)));
        }
    }
    alloc_free(self->alloc, block);
}
