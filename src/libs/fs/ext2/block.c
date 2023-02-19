#include <fs/ext2/block.h>
#include <fs/ext2/ext2.h>
#include <brutal-debug>

Ext2FsBlockResult ext2_acquire_blocks(Ext2Fs *self, size_t off, size_t count, FsBlockFlags flags)
{
    size_t block_diff = self->block_size / self->block_device->block_size;

    void *block = nullptr;

    fs_block_acquire(self->block_device, &block, count * block_diff, off * block_diff, flags);

    return OK(Ext2FsBlockResult, block);
}

void ext2_release_blocks(Ext2Fs *self, void *block)
{
    UNWRAP(fs_block_release(self->block_device, block));
}

static FsResult ext2_find_group_with_free_block(Ext2Fs *self, size_t *id, Ext2GroupBlockDescTable *group_desc)
{
    *id = 1;
    TRY(FsResult, ext2_group_read(self, group_desc, *id));
    size_t group_count = self->super_block->block_count / self->super_block->group_block_count;

    while ((*id) < group_count)
    {
        if (group_desc->unallocated_inodes_count == 0)
        {
            (*id)++;

            TRY(FsResult, ext2_group_read(self, group_desc, *id));
        }
        else
        {
            return OK(FsResult, (Success){});
        }
    }

    return ERR(FsResult, FS_NO_FREE_BLOCK);
}

static FsResult ext2_find_free_group_block(Ext2Fs *self, uint32_t group_bitmap_addr, size_t *result)
{
    Bits bit = {};
    uint32_t bitmap_block_count = self->super_block->group_block_count / (8 * self->block_size);
    uint8_t *bitmap = TRY(FsResult, ext2_acquire_blocks(self, group_bitmap_addr, bitmap_block_count, FS_BLOCK_READ | FS_BLOCK_WRITE));

    bits_init(&bit, bitmap, bitmap_block_count * self->block_size);

    *result = bits_find_free(&bit, 0, 1, false).base;

    bits_set(&bit, *result, true);

    ext2_release_blocks(self, bitmap);

    return OK(FsResult, (Success){});
}

FsResult ext2_alloc_block(Ext2Fs *self, uint32_t preferred_group_id, uint32_t *result)
{
    size_t group_id = preferred_group_id;
    Ext2GroupBlockDescTable group_desc = {};

    TRY(FsResult, ext2_group_read(self, &group_desc, group_id));

    // if we can't find a free block in this group, try to find a block in an other one
    if (group_desc.unallocated_block_count == 0)
    {
        TRY(FsResult, ext2_find_group_with_free_block(self, &group_id, &group_desc));
    }

    size_t allocated_block_idx = 0;
    TRY(FsResult, ext2_find_free_group_block(self, group_desc.block_usage_bitmap_addr, &allocated_block_idx));

    *result =
        ((self->block_size == 1024) ? 1 : 0) +
        (group_id * self->super_block->group_block_count) +
        (allocated_block_idx);

    group_desc.unallocated_block_count -= 1;

    return ext2_group_write(self, &group_desc, group_id);
}
