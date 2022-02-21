#pragma once

#include <brutal/io/traits.h>
#include <embed/file-decl.h>

typedef struct
{
    EmbedFile embed;
} IoFile;

typedef enum
{
    FILE_OPEN_READ_ONLY,
    FILE_OPEN_WRITE_ONLY,
    FILE_OPEN_READ_WRITE,
} FileOpenFlags;


MaybeError io_file_open(IoFile *self, Str path, FileOpenFlags flags);

MaybeError io_file_create(IoFile *self, Str path);

IoReader io_file_reader(IoFile *self);

IoWriter io_file_writer(IoFile *self);

IoDuplex io_file_duplex(IoFile *self);

IoSeeker io_file_seeker(IoFile *self);

IoRSeek io_file_rseek(IoFile *self);

IoWSeek io_file_wseek(IoFile *self);

MaybeError io_file_close(IoFile *self);
