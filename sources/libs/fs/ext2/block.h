#pragma once

#include <fs/ext2/ext2.h>

typedef Result(FsError, void *) Ext2FsBlockResult;

Ext2FsBlockResult ext2_acquire_blocks(Ext2Fs *self, size_t off, size_t count, FsBlockFlags flags);

void ext2_release_blocks(Ext2Fs *self, void *block);

FsResult ext2_alloc_block(Ext2Fs *self, uint32_t preferred_group_id, uint32_t *result);
