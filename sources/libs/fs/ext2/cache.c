
#include <fs/ext2/ext2.h>

Ext2CachedInode *get_cached_inode(Ext2Fs *self, Ext2FsInodeId inode)
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

Ext2CachedInode *add_cached_inode(Ext2Fs *self, Ext2FsInode inode)
{
    Ext2CachedInode cached = {
        .inode = inode,
    };

    vec_init(&cached.block, self->alloc);

    vec_push(&self->inode_cache, cached);
    return vec_end(&self->inode_cache);
}

Ext2CachedInode *get_cached_inode_or_create(Ext2Fs *self, Ext2FsInode inode)
{
    Ext2CachedInode *cached = get_cached_inode(self, inode.id);
    if (cached == nullptr)
    {
        return add_cached_inode(self, inode);
    }

    return cached;
}
