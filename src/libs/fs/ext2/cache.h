#pragma once

#include <fs/ext2/ext2.h>

Ext2CachedInode *get_cached_inode(Ext2Fs *self, Ext2FsInodeId inode);

Ext2CachedInode *add_cached_inode(Ext2Fs *self, Ext2FsInode inode);

Ext2CachedInode *get_cached_inode_or_create(Ext2Fs *self, Ext2FsInode inode);
