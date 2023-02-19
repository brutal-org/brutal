#include <fs/ext2/block.h>
#include <fs/ext2/cache.h>
#include <fs/ext2/ext2.h>
#include <brutal-debug>
#include <brutal-math>

static FsResult ext2_group(Ext2Fs *self, Ext2GroupBlockDescTable *group, uint64_t group_id, bool write)
{
    size_t block_off = group_id % self->block_size;
    size_t block_id = group_id / self->block_size + self->group_desc_table_block;

    Ext2GroupBlockDescTable *desc = TRY(FsResult, ext2_acquire_blocks(self, block_id, 1, write ? (FS_BLOCK_WRITE | FS_BLOCK_READ) : FS_BLOCK_READ));

    if (!write)
    {
        *group = desc[block_off];
    }
    else
    {
        desc[block_off] = *group;
    }

    ext2_release_blocks(self, desc);
    return OK(FsResult, (Success){});
}

static size_t ext2_group_from_inode(Ext2Fs *self, Ext2FsInode inode)
{
    return (inode.id - 1) / self->super_block->group_inode_count;
}

FsResult ext2_group_read(Ext2Fs *self, Ext2GroupBlockDescTable *group, uint64_t group_id)
{
    return ext2_group(self, group, group_id, false);
}

FsResult ext2_group_write(Ext2Fs *self, Ext2GroupBlockDescTable *group, uint64_t group_id)
{
    return ext2_group(self, group, group_id, true);
}

static uint64_t inode_size(Ext2Fs const *self, Ext2FsInode const *inode)
{
    uint64_t len = inode->block.size_low;
    if (self->super_block->read_only_feature & EXT2_FEAT_64BIT_FILE_SIZE)
    {
        len |= (uint64_t)inode->block.size_upper << 32;
    }

    return len;
}

FsResult ext2_inode_rw(Ext2Fs *self, Ext2FsInode *inode, Ext2FsInodeId id, bool write)
{
    if (id <= 1)
    {
        return ERR(FsResult, FS_INVALID_INODE);
    }

    Ext2CachedInode *cached = get_cached_inode(self, id);

    if (!write && cached != nullptr)
    {
        *inode = cached->inode;
        return OK(FsResult, (Success){});
    }
    else if (write)
    {
        if (cached == nullptr)
        {
            cached = add_cached_inode(self, *inode);
        }

        cached->inode = *inode;
    }

    size_t inode_id = (id - 1) % self->super_block->group_inode_count;
    size_t group_id = (id - 1) / self->super_block->group_inode_count;
    size_t inode_off_in_block = (inode_id * self->super_block->inode_size) % self->block_size;
    size_t inode_block = (inode_id * self->super_block->inode_size) / self->block_size;

    Ext2GroupBlockDescTable group_desc = {};

    TRY(FsResult, ext2_group_read(self, &group_desc, group_id));

    uint8_t *inode_block_data = TRY(FsResult, ext2_acquire_blocks(self, group_desc.inode_table_addr + inode_block, 1, write ? (FS_BLOCK_READ | FS_BLOCK_WRITE) : FS_BLOCK_READ));

    if (!write)
    {
        // we can't use Ext2InodeBlock as an array as inode_size can be != sizeof(Ext2InodeBlock)
        inode->block = *(Ext2InodeBlock *)(inode_block_data + inode_off_in_block);
        inode->id = id;

        add_cached_inode(self, *inode);
    }
    else
    {
        *(Ext2InodeBlock *)(inode_block_data + inode_off_in_block) = inode->block;
    }

    ext2_release_blocks(self, inode_block_data);

    return OK(FsResult, (Success){});
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

// this function read recursively every block
// here is an example if each block was containing 2 sub blocks
// the depth represent the distance we have from blocks containing the data
// you can se it as this:

// [-0--------][-1--------] depth 2
// [-0--][-1--][-2--][-3--] depth 1
// [0][1][2][3][4][5][6][7] depth 0

static FsResult ext2_inode_read_indirect_block_recursive(Ext2Fs *self, Ext2Blocks *result, size_t max_size, size_t depth, uint32_t block_ptr)
{
    if (depth == 0)
    {
        vec_push(result, block_ptr);
        return OK(FsResult, (Success){});
    }

    size_t entry_per_block = self->block_size / sizeof(uint32_t);
    size_t total_entry_in_current_depth = get_entry_in_indirect_block(entry_per_block, depth);
    size_t total_entry_in_child = total_entry_in_current_depth / entry_per_block;

    uint32_t *indirect_table_block = TRY(FsResult, ext2_acquire_blocks(self, block_ptr, 1, FS_BLOCK_READ));

    size_t end = m_min(align_up$(max_size, total_entry_in_child), total_entry_in_current_depth);

    for (size_t entry = 0; entry < end; entry += total_entry_in_child)
    {
        uint32_t child = indirect_table_block[entry / total_entry_in_child];
        FsResult res = ext2_inode_read_indirect_block_recursive(self, result, max_size, depth - 1, child);

        if (!res.succ)
        {
            ext2_release_blocks(self, indirect_table_block);
            return res;
        }
    }

    ext2_release_blocks(self, indirect_table_block);
    return OK(FsResult, (Success){});
}

FsResult ext2_inode_get_blocks(Ext2Fs *self, Ext2CachedInode *inode, size_t block_count)
{
    if (inode->block.len != 0)
    {
        return OK(FsResult, (Success){});
    }

    // ---- depth 0 ----

    uint32_t block_per_entry = self->block_size / sizeof(uint32_t);

    for (size_t i = 0; i < m_min(12, block_count); i++)
    {
        vec_push(&inode->block, inode->inode.block.blocks_ptr[i]);
    }

    if (block_count < 12)
    {
        return OK(FsResult, (Success){});
    }

    block_count -= 12;

    // ---- depth 1 ----

    TRY(FsResult, ext2_inode_read_indirect_block_recursive(self, &inode->block, block_count, 1, inode->inode.block.single_indirect_block_ptr));

    if (block_count < block_per_entry)
    {
        return OK(FsResult, (Success){});
    }

    block_count -= block_per_entry;

    // ---- depth 2 ----

    TRY(FsResult, ext2_inode_read_indirect_block_recursive(self, &inode->block, block_count, 2, inode->inode.block.double_indirect_block_ptr));

    if (block_count < block_per_entry * block_per_entry)
    {
        return OK(FsResult, (Success){});
    }

    block_count -= block_per_entry * block_per_entry;

    // ---- depth 3 ----

    return ext2_inode_read_indirect_block_recursive(self, &inode->block, block_count, 3, inode->inode.block.triple_indirect_block_ptr);
}

FsResult ext2_inode_read(Ext2Fs *self, const Ext2FsInode *inode, void *data, BlockRange range)
{
    Ext2CachedInode *cached_inode = get_cached_inode_or_create(self, *inode);

    size_t inode_block_count = align_up$(inode_size(self, inode), self->block_size) / self->block_size;

    TRY(FsResult, ext2_inode_get_blocks(self, cached_inode, inode_block_count));

    for (long i = range.base; i < range_end(range); i++)
    {
        void *block_data = TRY(FsResult, ext2_acquire_blocks(self, cached_inode->block.data[i], 1, FS_BLOCK_READ));

        mem_cpy(data, block_data, self->block_size);

        ext2_release_blocks(self, block_data);

        data += self->block_size;
    }

    return OK(FsResult, (Success){});
}

static FsResult ext2_inode_push_block_rec(Ext2Fs *self, Ext2CachedInode *inode, size_t new_block_ptr, size_t block, size_t depth, size_t idx)
{
    size_t block_per_level = self->block_size / sizeof(uint32_t);

    size_t block_index = idx / get_entry_in_indirect_block(block_per_level, depth - 1);

    uint32_t *table = TRY(FsResult, ext2_acquire_blocks(self, block, 1, FS_BLOCK_READ | FS_BLOCK_WRITE));

    if (depth != 0)
    {
        if (table[block_index] == 0)
        {
            size_t group_id = ext2_group_from_inode(self, inode->inode);
            TRY(FsResult, ext2_alloc_block(self, group_id, &table[block_index]));
        }

        TRY(FsResult, ext2_inode_push_block_rec(self, inode, new_block_ptr, table[block_index], depth - 1, idx));
    }
    else
    {
        table[block_index] = new_block_ptr;
    }

    ext2_release_blocks(self, table);
    return OK(FsResult, (Success){});
}

static FsResult ext2_inode_insert_block(Ext2Fs *self, Ext2CachedInode *inode, size_t new_block_ptr, uint32_t idx)
{

    size_t block_per_entry = self->block_size / sizeof(uint32_t);

    // ----- depth 0 -----

    if (idx < 12)
    {
        inode->inode.block.blocks_ptr[idx] = new_block_ptr;
        return OK(FsResult, (Success){});
    }

    idx -= 12;

    // ----- depth 1 -----

    if (idx < block_per_entry)
    {
        return ext2_inode_push_block_rec(self, inode, new_block_ptr, inode->inode.block.single_indirect_block_ptr, 1, idx);
    }

    idx -= block_per_entry;

    // ----- depth 2 -----

    if (idx < block_per_entry * block_per_entry)
    {
        return ext2_inode_push_block_rec(self, inode, new_block_ptr, inode->inode.block.double_indirect_block_ptr, 2, idx);
    }

    idx -= block_per_entry * block_per_entry;

    // ----- depth 3 -----

    if (idx < block_per_entry * block_per_entry * block_per_entry)
    {
        return ext2_inode_push_block_rec(self, inode, new_block_ptr, inode->inode.block.triple_indirect_block_ptr, 3, idx);
    }

    panic$("inode size overflow: {#x}", idx);
}

static FsResult ext2_inode_grow(Ext2Fs *self, Ext2CachedInode *inode, size_t new_block_count)
{
    uint32_t group_id = ext2_group_from_inode(self, inode->inode);

    for (size_t i = 0; i < new_block_count; i++)
    {
        uint32_t new_block = 0;

        TRY(FsResult, ext2_alloc_block(self, group_id, &new_block));

        assert_not_equal(new_block, (uint32_t)0);

        uint64_t idx = align_up$(inode_size(self, &inode->inode), self->block_size) / self->block_size + i;

        TRY(FsResult, ext2_inode_insert_block(self, inode, new_block, idx));
    }

    return OK(FsResult, (Success){});
}

FsResult ext2_write_inode(Ext2Fs *self, Ext2FsInode *inode)
{
    return ext2_inode_rw(self, inode, inode->id, true);
}

FsResult ext2_inode_write(Ext2Fs *self, Ext2FsInode *inode, const void *data, BlockRange range)
{
    Ext2CachedInode *cached_inode = get_cached_inode_or_create(self, *inode);

    // allocating new block if it is necessary
    if (inode_size(self, inode) < range_end(range) * self->block_size)
    {
        // the size can be increased without allocate any newer block
        if (align_up$(inode_size(self, inode), self->block_size) / self->block_size > (size_t)range_end(range))
        {
            inode->block.size_low = range_end(range) * self->block_size;
            TRY(FsResult, ext2_write_inode(self, inode));
        }
        else
        {
            size_t new_block_count = range_end(range) - align_up$(inode_size(self, inode), self->block_size) / self->block_size;
            TRY(FsResult, ext2_inode_grow(self, cached_inode, new_block_count));

            inode->block.size_low = range_end(range) * self->block_size;
            TRY(FsResult, ext2_write_inode(self, inode));
        }
    }

    vec_deinit(&cached_inode->block);
    vec_init(&cached_inode->block, self->alloc);
    TRY(FsResult, ext2_inode_get_blocks(self, cached_inode, align_up$(inode->block.size_low, self->block_size) / self->block_size));

    for (long i = range.base; i < range_end(range); i++)
    {
        assert_not_equal(cached_inode->block.data[i], (uint32_t)0);
        uint8_t *block = TRY(FsResult, ext2_acquire_blocks(self, cached_inode->block.data[i], 1, FS_BLOCK_WRITE));

        mem_cpy(block, data, self->block_size);

        ext2_release_blocks(self, block);

        data += self->block_size;
    }

    return OK(FsResult, (Success){});
}

FsResult ext2_init(Ext2Fs *self, FsBlockImpl *block, Alloc *alloc)
{
    assert_equal((size_t)(block->block_size % 512), (size_t)0);

    *self = (Ext2Fs){};
    self->block_device = block;
    self->alloc = alloc;

    fs_block_acquire(block, (void **)&self->super_block, EXT2_SUPER_BLOCK_BASE / block->block_size, EXT2_SUPER_BLOCK_SIZE / block->block_size, FS_BLOCK_READ);

    if (self->super_block->signature != EXT2_SIGNATURE)
    {
        log$("invalid ext2 fs signature");

        return ERR(FsResult, FS_INVALID_FS);
    }

    self->block_size = 1 << (self->super_block->block_size + 10);

    if (self->block_size < self->block_device->block_size)
    {
        log$("not supported: fs device with higher block size than ext2fs");

        return ERR(FsResult, FS_INVALID_FS);
    }

    ext2_dump_superblock(self->super_block);

    self->group_desc_table_block = self->block_size == 1024 ? 2 : 1;

    self->group_count = self->super_block->inode_count / self->super_block->group_inode_count;

    vec_init(&self->inode_cache, self->alloc);

    return OK(FsResult, (Success){});
}

FsResult ext2_inode(Ext2Fs *self, Ext2FsInode *inode, Ext2FsInodeId id)
{
    return ext2_inode_rw(self, inode, id, false);
}

Iter ext2_fs_iter(Ext2Fs *self, Ext2FsInode *inode, Ext2IterFileFn fn, void *ctx)
{
    if (!((inode->block.mode >> 12) == EXT2_INODE_DIRECTORY))
    {
        return ITER_CONTINUE;
    }
    uint64_t size = inode_size(self, inode);

    void *block_data = alloc_malloc(self->alloc, self->block_size);

    for (uint64_t i = 0; i < size; i += self->block_size)
    {
        UNWRAP(ext2_inode_read(self, inode, block_data, (BlockRange){.base = i / self->block_size, .size = 1}));

        Ext2Directory *dir = block_data;

        while (dir->type != EXT2_FT_UNKOWN && (uintptr_t)dir - (uintptr_t)block_data < self->block_size)
        {
            Ext2FsFile sub_file = {};

            UNWRAP(ext2_inode(self, &sub_file.inode, dir->inode));

            sub_file.name = str_n$(dir->name_len, (char *)dir->name_chars);

            if (fn(&sub_file, ctx) == ITER_STOP)
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
