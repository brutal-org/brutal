#pragma once

typedef enum
{
    FS_SUCCESS,
    FS_ERROR_FROM_BLOCK,
    FS_NOT_FOUND,
    FS_INVALID_INODE,
    FS_INVALID_PATH,
    FS_INVALID_FS,
    FS_NO_FREE_BLOCK
} FsError;
