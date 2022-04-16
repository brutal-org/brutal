#include <fs/ext2/structure.h>
#include <brutal-debug>

static void ext2_dump_ro_features(Ext2SuperBlock *superblock)
{
    log$("Superblock Read only features: ");

    if (superblock->read_only_feature & EXT2_FEAT_SPARSE_BLOCKS)
    {
        log$(" - Sparse super block and group descriptor tables ");
    }
    if (superblock->read_only_feature & EXT2_FEAT_64BIT_FILE_SIZE)
    {
        log$(" - 64 bit file size");
    }
    if (superblock->read_only_feature & EXT2_FEAT_BINARY_TREE_STORAGE)
    {
        log$(" - Binary tree storage");
    }
}

static void ext2_dump_optional_features(Ext2SuperBlock *superblock)
{
    log$("Superblock Optional features: ");

    if (superblock->optional_features & EXT2_FEAT_RESERVE_BLOCK)
    {
        log$(" - Reserve block for directory ");
    }
    if (superblock->optional_features & EXT2_FEAT_AFS_SERVER)
    {
        log$(" - AFS server inode");
    }
    if (superblock->optional_features & EXT2_FEAT_JOURNAL)
    {
        log$(" - Journal");
    }
    if (superblock->optional_features & EXT2_FEAT_EXTENDED_INODE_ATTRIBUTES)
    {
        log$(" - Extended inode attributes");
    }
    if (superblock->optional_features & EXT2_FEAT_RESIZABLE_FS)
    {
        log$(" - Resizable file system");
    }
    if (superblock->optional_features & EXT2_FEAT_HASH_IDX)
    {
        log$(" - Hash path");
    }
}

static void ext2_dump_required_features(Ext2SuperBlock *superblock)
{
    log$("Superblock required features: ");

    if (superblock->required_features & EXT2_FEAT_COMPRESSION)
    {
        log$(" - Compression ");
    }
    if (superblock->required_features & EXT2_FEAT_COMPRESSION)
    {
        log$(" - Type directory");
    }
    if (superblock->required_features & EXT2_FEAT_JOURNAL_REPLAY)
    {
        log$(" - Journal replay");
    }
    if (superblock->required_features & EXT2_FEAT_JOURNAL_DEVICE)
    {
        log$(" - Journal device");
    }
}

void ext2_dump_superblock(Ext2SuperBlock *superblock)
{
    log$("Superblock dump:");

    log$(" - Ext2 version: {}.{}", superblock->version_major, superblock->version_minor);
    log$(" - Inode count: {}", superblock->inode_count);
    log$(" - Block count: {}", superblock->block_count);
    log$(" - Inode structure size: {}", superblock->inode_size);
    log$(" - Block size: {}", 1 << (superblock->block_size + 10));
    log$(" - Free Inode count: {}", superblock->free_inode_count);
    log$(" - Free Block count: {}", superblock->free_block_count);

    ext2_dump_optional_features(superblock);
    ext2_dump_ro_features(superblock);
    ext2_dump_required_features(superblock);
}

const char *file_type_to_str[] = {
    [EXT2_INODE_FIFO] = "fifo",
    [EXT2_INODE_CHARDEV] = "chardev",
    [EXT2_INODE_DIRECTORY] = "directory",
    [EXT2_INODE_BLOCK_DEVICE] = "block device",
    [EXT2_INODE_REGULAR_FILE] = "regular file",
    [EXT2_INODE_LINK] = "link",
    [EXT2_INODE_UNIX_SOCKET] = "unix socket",
};

void ext2_dump_inodeblock(Ext2InodeBlock *block)
{
    log$("Inode block dump:");
    log$(" - is: {} ({})", block->mode >> 12, file_type_to_str[block->mode >> 12]);
    log$(" - size: {}", block->size_low);
    log$(" - directory-count: {}", block->directory_count);
}
