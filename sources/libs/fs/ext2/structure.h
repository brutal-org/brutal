#pragma once

#include <brutal-base>

#define EXT2_SIGNATURE 0xef53

#define EXT2_SUPER_BLOCK_BASE 1024
#define EXT2_SUPER_BLOCK_SIZE 1024

typedef enum
{
    EXT2_STATE_CLEAN = 1,
    EXT2_STATE_ERROR = 2,
} Ext2FsState;

typedef enum
{
    EXT2_ERROR_IGNORE = 1,
    EXT2_ERROR_REMOUNT_IN_RO = 2,
    EXT2_ERROR_KERNEL_PANIC = 3,
} Ext2FsErrorHandling;

typedef enum
{
    EXT2_OS_LINUX = 0,
    EXT2_OS_HURD = 1,
    EXT2_OS_MASIX = 2,
    EXT2_OS_FREEBSD = 3,
    EXT2_OS_LITES = 4,
    /*  EXT2_OS_BRUTAL = 5,  one day maybe ? */
} Ext2FsOSId;

typedef enum
{
    EXT2_FEAT_RESERVE_BLOCK = 1 << 0,
    EXT2_FEAT_AFS_SERVER = 1 << 1,
    EXT2_FEAT_JOURNAL = 1 << 2,
    EXT2_FEAT_EXTENDED_INODE_ATTRIBUTES = 1 << 3,
    EXT2_FEAT_RESIZABLE_FS = 1 << 4,
    EXT2_FEAT_HASH_IDX = 1 << 5,
} Ext2OptionalFeatures;

typedef enum
{
    EXT2_FEAT_COMPRESSION = 1 << 0,
    EXT2_FEAT_TYPE_DIRECTORY = 1 << 1,
    EXT2_FEAT_JOURNAL_REPLAY = 1 << 2,
    EXT2_FEAT_JOURNAL_DEVICE = 1 << 3,
} Ext2RequiredFeatures;

typedef enum
{
    EXT2_FEAT_SPARSE_BLOCKS = 1 << 0,
    EXT2_FEAT_64BIT_FILE_SIZE = 1 << 1,
    EXT2_FEAT_BINARY_TREE_STORAGE = 1 << 2,
} Ext2ReadOnlyFeatures;

typedef struct
{
    uint32_t inode_count;
    uint32_t block_count;
    uint32_t reserved_block_count;
    uint32_t free_block_count;
    uint32_t free_inode_count;
    uint32_t super_block_id;
    uint32_t block_size;    // real block size = 2 ** (10 + value)
    uint32_t fragment_size; // same formula as block_size
    uint32_t group_block_count;
    uint32_t group_fragment_count;
    uint32_t group_inode_count;
    uint32_t last_mount_time;
    uint32_t last_write_time;
    uint16_t mount_count_since_last_check;
    uint16_t mount_count_before_check;
    uint16_t signature;
    uint16_t fs_state;
    uint16_t error_handling;
    uint16_t version_minor;
    uint32_t time_since_last_check;
    uint32_t time_before_forced_check;
    uint32_t creator_os_id;
    uint32_t version_major;
    uint16_t user_id_reserved_block;
    uint16_t group_id_reserved_block;

    // only exist if major version is greater or equal to 1
    uint32_t first_non_reserved_inode;
    uint16_t inode_size;
    uint16_t superblock_group_location;
    uint32_t optional_features;
    uint32_t required_features;
    uint32_t read_only_feature;
    uint8_t file_system_id[16];
    uint8_t volume_name[16];
    uint8_t mount_path[64];
    uint32_t compression_algorithm;
    uint8_t preallocated_file_block_count;
    uint8_t preallocated_dir_block_count;
    uint16_t _unused;

    uint8_t journal_id[16];
    uint32_t journal_inode;
    uint32_t journal_device;
    uint32_t orphaned_inode_list;
    uint8_t _unused2[788];

} Ext2SuperBlock;

typedef struct
{
    uint32_t block_usage_bitmap_addr;
    uint32_t inode_usage_bitmap_addr;
    uint32_t inode_table_addr;
    uint16_t unallocated_block_count;
    uint16_t unallocated_inodes_count;
    uint16_t dir_count;
    uint16_t _reserved[7];
} Ext2GroupBlockDescTable;

typedef enum
{
    EXT2_INODE_FIFO = 0x1,
    EXT2_INODE_CHARDEV = 0x2,
    EXT2_INODE_DIRECTORY = 0x4,
    EXT2_INODE_BLOCK_DEVICE = 0x6,
    EXT2_INODE_REGULAR_FILE = 0x8,
    EXT2_INODE_LINK = 0xA,
    EXT2_INODE_UNIX_SOCKET = 0xC,
} Ext2InodeType;

typedef struct PACKED
{
    uint16_t mode;
    uint16_t user_id;
    uint32_t size_low;
    uint32_t last_access_time;
    uint32_t creation_time;
    uint32_t last_modification_time;
    uint32_t deletion_time;
    uint16_t group_id;
    uint16_t directory_count;
    uint32_t disk_sector_usage_count;
    uint32_t flags;
    uint32_t os_specific_value;
    uint32_t blocks_ptr[12];
    uint32_t single_indirect_block_ptr;
    uint32_t double_indirect_block_ptr;
    uint32_t triple_indirect_block_ptr;
    uint32_t generation_number;
    uint32_t extended_attribute_block;
    uint32_t size_upper;
    uint32_t fragment_addr;
    uint8_t os_specific_2[12];
} Ext2InodeBlock;

typedef enum
{
    EXT2_FT_UNKOWN = 0,
    EXT2_FT_REGULAR_FILE = 1,
    EXT2_FT_DIRECTORY = 2,
    EXT2_FT_CHARDEV = 3,
    EXT2_FT_BLOCK_DEV = 4,
    EXT2_FT_FIFO = 5,
    EXT2_FT_SOCKET = 6,
    EXT2_FT_SYMLINK = 7,
} Ext2FileType;

typedef struct
{
    uint32_t inode;
    uint16_t size;
    uint8_t name_len;
    uint8_t type;
    uint8_t name_chars[255];
} Ext2Directory;

void ext2_dump_superblock(Ext2SuperBlock *superblock);

void ext2_dump_inodeblock(Ext2InodeBlock *block);
