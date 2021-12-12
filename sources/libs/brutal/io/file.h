#pragma once

#include <brutal/io/read.h>
#include <brutal/io/write.h>
#include <embed/file.h>

typedef struct
{
    EmbedFile embed;
} IoFile;

MaybeError io_file_open(IoFile *self, Str path);

MaybeError io_file_create(IoFile *self, Str path);

IoReader io_file_reader(IoFile *self);

IoWriter io_file_writer(IoFile *self);

MaybeError io_file_close(IoFile *self);
