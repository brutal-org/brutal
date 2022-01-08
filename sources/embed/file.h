#pragma once

#include <brutal/io/file.h>

MaybeError embed_file_open(IoFile *self, Str path);

MaybeError embed_file_create(IoFile *self, Str path);

MaybeError embed_file_close(IoFile *self);

IoResult embed_file_seek(IoFile *self, IoSeek pos);

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

IoResult embed_file_read(IoFile *self, uint8_t *data, size_t size);

IoResult embed_file_write(IoFile *self, uint8_t const *data, size_t size);
