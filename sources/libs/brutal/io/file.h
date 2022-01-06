#pragma once

#include <brutal/io/rwseek.h>
#include <embed/file-decl.h>

typedef struct
{
    EmbedFile embed;
} IoFile;

MaybeError io_file_open(IoFile *self, Str path);

MaybeError io_file_create(IoFile *self, Str path);

IoReader io_file_reader(IoFile *self);

IoWriter io_file_writer(IoFile *self);

IoDuplex io_file_duplex(IoFile *self);

IoSeeker io_file_seeker(IoFile *self);

IoRwSeek io_file_rwseek(IoFile *self);

MaybeError io_file_close(IoFile *self);
