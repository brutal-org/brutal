#include <brutal/debug.h>
#include <brutal/math.h>
#include <fs/ext2/ext2.h>

static Ext2CachedInode *get_cached_inode(Ext2Fs *self, Ext2FsInodeId inode)
{
    vec_foreach(cached, &self->inode_cache)
    {
        if (cached->inode.id == inode)
        {
            return cached;
        }
    }
    return nullptr;
}

static Ext2CachedInode *add_cached_inode(Ext2Fs *self, Ext2FsInode inode)
{
    Ext2CachedInode cached = {
        .inode = inode,
    };

    vec_init(&cached.block, self->alloc);

    vec_push(&self->inode_cache, cached);
    return vec_end(&self->inode_cache);
}

static Ext2CachedInode *get_cached_inode_or_create(Ext2Fs *self, Ext2FsInode inode)
{
    Ext2CachedInode *cached = get_cached_inode(self, inode.id);
    if (cached == nullptr)
    {
        return add_cached_inode(self, inode);
    }

    return cached;
}

static void *ext2_acquire_blocks(Ext2Fs *self, size_t off, size_t count, FsBlockFlags flags)
{
    size_t block_diff = self->block_size / self->block_device->block_size;

    void *block = nullptr;
    fs_block_acquire(self->block_device, &block, count * block_diff, off * block_diff, flags);
    return block;
}

static void ext2_release_blocks(Ext2Fs *self, void *block)
{
    fs_block_release(self->block_device, block);
}

static FsResult ext2_group(Ext2Fs *self, Ext2GroupBlockDescTable *group, uint64_t group_id, bool write)
{
    size_t block_off = group_id % self->block_size;
    size_t block_id = group_id / self->block_size + self->group_desc_table_block;

    Ext2GroupBlockDescTable *desc = ext2_acquire_blocks(self, block_id, 1, write ? FS_BLOCK_WRITE : FS_BLOCK_READ);
    if (desc == nullptr)
    {
        return FS_ERROR_FROM_BLOCK;
    }

    if (!write)
    {
        *group = desc[block_off];
    }
    else
    {
        desc[block_off] = *group;
    }

    ext2_release_blocks(self, desc);
    return FS_SUCCESS;
}

static FsResult ext2_group_read(Ext2Fs *self, Ext2GroupBlockDescTable *group, uint64_t group_id)
{
    return ext2_group(self, group, group_id, false);
}

static FsResult ext2_group_write(Ext2Fs *self, Ext2GroupBlockDescTable *group, uint64_t group_id)
{
    return ext2_group(self, group, group_id, true);
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

// [-0--------------------] depth 3
// [-0--------][-1--------] depth 2
// [-0--][-1--][-2--][-3--] depth 1
// [0][1][2][3][4][5][6][7] depth 0

static FsResult ext2_inode_read_indirect_block_recursive(Ext2Fs *self, Ext2Blocks *result, size_t max_size, size_t depth, uint32_t block_ptr)
{
    if (depth == 0)
    {
        vec_push(result, block_ptr);
        return FS_SUCCESS;
    }

    // entry for depth = 0
    size_t entry_per_block = self->block_size / sizeof(uint32_t);
    // total entry for depth
    size_t entry_in_indirect_rec = get_entry_in_indirect_block(entry_per_block, depth);
    // total entry for (depth-1)
    size_t block_per_entry_rec = entry_in_indirect_rec / entry_per_block;

    uint32_t *indirect_table_block = ext2_acquire_blocks(self, block_ptr, 1, FS_BLOCK_READ);

    for (size_t entry = 0; entry < m_min(align_up$(max_size, block_per_entry_rec), entry_in_indirect_rec); entry += block_per_entry_rec)
    {
        uint32_t next_block = indirect_table_block[entry / block_per_entry_rec];
        FsResult res = ext2_inode_read_indirect_block_recursive(self, result, max_size, depth - 1, next_block);
        if (res != FS_SUCCESS)
        {
            ext2_release_blocks(self, indirect_table_block);
            return res;
        }
    }
    ext2_release_blocks(self, indirect_table_block);
    return FS_SUCCESS;
}

FsResult ext2_inode_get_blocks(Ext2Fs *self, Ext2CachedInode *inode, size_t block_count)
{
    if (inode->block.len != 0)
    {
        return FS_SUCCESS;
    }

    uint32_t block_per_entry = self->block_size / sizeof(uint32_t);
    for (size_t i = 0; i < m_min(12, block_count); i++)
    {
        vec_push(&inode->block, inode->inode.block.blocks_ptr[i]);
    }

    if (block_count < 12)
    {
        return FS_SUCCESS;
    }

    block_count -= 12;

    FsResult res = ext2_inode_read_indirect_block_recursive(self, &inode->block, block_count, 1, inode->inode.block.single_indirect_block_ptr);
    if (res != FS_SUCCESS)
    {
        return res;
    }

    if (block_count < block_per_entry)
    {
        return res;
    }
    block_count -= block_per_entry;

    res = ext2_inode_read_indirect_block_recursive(self, &inode->block, block_count, 2, inode->inode.block.double_indirect_block_ptr);
    if (res != FS_SUCCESS)
    {
        return res;
    }

    if (block_count < block_per_entry * block_per_entry)
    {
        return res;
    }

    block_count -= block_per_entry * block_per_entry;

    res = ext2_inode_read_indirect_block_recursive(self, &inode->block, block_count, 3, inode->inode.block.triple_indirect_block_ptr);

    if (res != FS_SUCCESS)
    {
        return res;
    }

    if (block_count < block_per_entry * block_per_entry * block_per_entry)
    {
        return res;
    }

    panic$("block count: {#x} is too big, don't have support for more than triple block indirection", block_count);
}

FsResult ext2_inode_read(Ext2Fs *self, const Ext2FsInode *inode, void *data, BlockRange range)
{
    Ext2CachedInode *cached_inode = get_cached_inode_or_create(self, *inode);

    if (ext2_inode_get_blocks(self, cached_inode, align_up$(inode->block.size_low, self->block_size) / self->block_size) != FS_SUCCESS)
    {
        panic$("unable to retreive block for inode: {#x}", inode->id);
    }

    for (int i = range.base; i < range_end(range); i++)
    {
        uint32_t *block = ext2_acquire_blocks(self, cached_inode->block.data[i], 1, FS_BLOCK_READ);

        mem_cpy(data, block, self->block_size);

        ext2_release_blocks(self, block);

        data += self->block_size;
    }

    return FS_SUCCESS;
}

static FsResult ext2_find_group_with_free_block(Ext2Fs *self, size_t *id, Ext2GroupBlockDescTable *group_desc)
{
    *id = 1;
    FsResult res = ext2_group_read(self, group_desc, *id);

    while (res == FS_SUCCESS)
    {
        if (group_desc->unallocated_inodes_count == 0)
        {
            (*id)++;

            res = ext2_group_read(self, group_desc, *id);
        }
        else
        {
            return res;
        }
    }

    return res;
}

static FsResult ext2_find_free_group_entry(Ext2Fs *self, uint32_t group_bitmap_addr, size_t *result)
{
    uint32_t bitmap_block_count = self->super_block->group_block_count / (8 * self->block_size);

    uint8_t *bitmap = ext2_acquire_blocks(self, group_bitmap_addr, bitmap_block_count, FS_BLOCK_READ | FS_BLOCK_WRITE);

    Bits bit = {};

    bits_init(&bit, bitmap, bitmap_block_count * self->block_size);

    *result = bits_find_free(&bit, 0, 1, false).base;

    bits_set(&bit, *result, true);

    ext2_release_blocks(self, bitmap);

    return FS_SUCCESS;
}

static FsResult ext2_alloc_block(Ext2Fs *self, uint32_t preferred_group_id, uint32_t *result)
{
    size_t group_id = preferred_group_id;
    Ext2GroupBlockDescTable group_desc = {};
    FsResult res = ext2_group_read(self, &group_desc, group_id);

    if (res != FS_SUCCESS)
    {
        return res;
    }

    // if we can't find a free block in this group, try to find a block in an other one
    if (group_desc.unallocated_block_count == 0)
    {
        res = ext2_find_group_with_free_block(self, &group_id, &group_desc);
    }

    size_t group_block_index = 0;
    res = ext2_find_free_group_entry(self, group_desc.block_usage_bitmap_addr, &group_block_index);

    size_t final_block = (self->block_size == 1024) ? 1 : 0;
    final_block += (group_id - 1) * self->super_block->group_block_count + group_block_index;
    *result = final_block;

    group_desc.unallocated_block_count -= 1;

    ext2_group_write(self, &group_desc, group_id);

    return res;
}

static FsResult ext2_inode_push_block_rec(Ext2Fs *self, Ext2CachedInode *inode, size_t new_block_ptr, size_t parent_block, size_t depth, size_t idx)
{
    size_t block_per_level = self->block_size / sizeof(uint32_t);

    size_t block_index = idx / get_entry_in_indirect_block(block_per_level, depth);

    uint32_t *table = ext2_acquire_blocks(self, parent_block, 1, FS_BLOCK_READ | FS_BLOCK_WRITE);

    if (depth != 0)
    {
        if (table[block_index] == 0)
        {
            size_t group_id = (inode->inode.id - 1) / self->super_block->group_inode_count;

            ext2_alloc_block(self, group_id, &table[block_index]);
        }

        ext2_inode_push_block_rec(self, inode, new_block_ptr, parent_block, depth - 1, idx);
    }
    else
    {
        table[block_index] = new_block_ptr;
    }

    ext2_release_blocks(self, table);
    return FS_SUCCESS;
}

static FsResult ext2_inode_push_block(Ext2Fs *self, Ext2CachedInode *inode, size_t new_block_ptr)
{

    size_t inode_pushed_idx = align_up$(inode->inode.block.size_low, self->block_size) / self->block_size + 1;
    size_t block_per_entry = self->block_size / sizeof(uint32_t);
    if (inode_pushed_idx < 12)
    {
        inode->inode.block.blocks_ptr[inode_pushed_idx] = new_block_ptr;
        return FS_SUCCESS;
    }

    inode_pushed_idx -= 12;

    if (inode_pushed_idx < block_per_entry)
    {
        return ext2_inode_push_block_rec(self, inode, new_block_ptr, inode->inode.block.single_indirect_block_ptr, 1, inode_pushed_idx);
    }

    inode_pushed_idx -= block_per_entry;

    if (inode_pushed_idx < block_per_entry * block_per_entry)
    {
        return ext2_inode_push_block_rec(self, inode, new_block_ptr, inode->inode.block.single_indirect_block_ptr, 2, inode_pushed_idx);
    }
    inode_pushed_idx -= block_per_entry * block_per_entry;

    if (inode_pushed_idx < block_per_entry * block_per_entry * block_per_entry)
    {
        return ext2_inode_push_block_rec(self, inode, new_block_ptr, inode->inode.block.single_indirect_block_ptr, 2, inode_pushed_idx);
    }

    panic$("inode size overflow: {#x}", inode_pushed_idx);
}

static FsResult ext2_inode_grow(Ext2Fs *self, Ext2CachedInode *inode, size_t new_block_count)
{
    size_t group_id = (inode->inode.id - 1) / self->super_block->group_inode_count;
    for (size_t i = 0; i < new_block_count; i++)
    {
        uint32_t block = 0;
        if (ext2_alloc_block(self, group_id, &block) != FS_SUCCESS)
        {
            return -1; // TODO: add more fs error
        }

        ext2_inode_push_block(self, inode, block);
    }
    return FS_SUCCESS;
}

FsResult ext2_write_inode(Ext2Fs *self, Ext2FsInode *inode)
{

    Ext2CachedInode *cached = get_cached_inode(self, inode->id);
    if (cached == nullptr)
    {
        panic$("trying to write a non existant inode");
    }
    cached->inode = *inode;

    size_t inode_id = (inode->id - 1) % self->super_block->group_inode_count;
    size_t group_id = (inode->id - 1) / self->super_block->group_inode_count;
    size_t inode_off_in_block = (inode_id * self->super_block->inode_size) % self->block_size;
    size_t inode_block = (inode_id * self->super_block->inode_size) / self->block_size;

    Ext2GroupBlockDescTable group_desc = {};
    FsResult res = ext2_group_read(self, &group_desc, group_id);
    if (res != FS_SUCCESS)
    {
        return res;
    }

    uint8_t *inode_block_data = ext2_acquire_blocks(self, group_desc.inode_table_addr + inode_block, 1, FS_BLOCK_READ | FS_BLOCK_WRITE);

    if (inode_block_data == nullptr)
    {
        return FS_ERROR_FROM_BLOCK;
    }

    // we can't use Ext2InodeBlock as an array as inode_size can be != sizeof(Ext2InodeBlock)
    *(Ext2InodeBlock *)(inode_block_data + inode_off_in_block) = inode->block;

    ext2_release_blocks(self, inode_block_data);

    return FS_SUCCESS;
}

FsResult ext2_inode_write(Ext2Fs *self, Ext2FsInode *inode, const void *data, BlockRange range)
{
    Ext2CachedInode *cached_inode = get_cached_inode_or_create(self, *inode);

    // allocating new block if it is necessary
    if (inode->block.size_low < range_end(range) * self->block_size)
    {

        // the size can be increased without allocate any newer block
        if (align_up$(inode->block.size_low, self->block_size) / self->block_size >= (size_t)range_end(range))
        {
            inode->block.size_low = range_end(range) * self->block_size;
            ext2_write_inode(self, inode);
        }
        else
        {
            size_t new_block_count = align_up$(range_end(range) * self->block_size - inode->block.size_low, self->block_size);
            ext2_inode_grow(self, cached_inode, new_block_count / self->block_size);

            inode->block.size_low = range_end(range) * self->block_size;
            ext2_write_inode(self, inode);
        }
    }

    if (ext2_inode_get_blocks(self, cached_inode, align_up$(inode->block.size_low, self->block_size) / self->block_size) != FS_SUCCESS)
    {
        panic$("unable to retreive block for inode: {#x}", inode->id);
    }

    for (int i = range.base; i < range_end(range); i++)
    {
        uint32_t *block = ext2_acquire_blocks(self, cached_inode->block.data[i], 1, FS_BLOCK_WRITE);

        mem_cpy(block, data, self->block_size);

        ext2_release_blocks(self, block);

        data += self->block_size;
    }

    return FS_SUCCESS;
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

    vec_init(&self->inode_cache, self->alloc);
    return FS_SUCCESS;
}

FsResult ext2_inode(Ext2Fs *self, Ext2FsInode *inode, Ext2FsInodeId id)
{
    if (id <= 1)
    {
        return FS_INVALID_INODE;
    }

    Ext2CachedInode *cached = get_cached_inode(self, id);

    if (cached != nullptr)
    {
        *inode = cached->inode;
        return FS_SUCCESS;
    }

    size_t inode_id = (id - 1) % self->super_block->group_inode_count;
    size_t group_id = (id - 1) / self->super_block->group_inode_count;
    size_t inode_off_in_block = (inode_id * self->super_block->inode_size) % self->block_size;
    size_t inode_block = (inode_id * self->super_block->inode_size) / self->block_size;

    Ext2GroupBlockDescTable group_desc = {};
    FsResult res = ext2_group_read(self, &group_desc, group_id);
    if (res != FS_SUCCESS)
    {
        return res;
    }

    uint8_t *inode_block_data = ext2_acquire_blocks(self, group_desc.inode_table_addr + inode_block, 1, FS_BLOCK_READ);

    if (inode_block_data == nullptr)
    {
        return FS_ERROR_FROM_BLOCK;
    }

    // we can't use Ext2InodeBlock as an array as inode_size can be != sizeof(Ext2InodeBlock)
    inode->block = *(Ext2InodeBlock *)(inode_block_data + inode_off_in_block);
    inode->id = id;

    add_cached_inode(self, *inode);

    ext2_release_blocks(self, inode_block_data);

    return FS_SUCCESS;
}

Iter ext2_fs_iter(Ext2Fs *self, Ext2FsInode *inode, Ext2IterFileFn fn, void *ctx)
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
            Ext2FsFile sub_file;
            ext2_inode(self, &sub_file.inode, dir->inode);
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
