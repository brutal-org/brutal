#pragma once

#include <brutal/host/io.h>
#include <brutal/io/read.h>
#include <brutal/io/write.h>

typedef struct
{
    HostIoFileHandle handle;
} IoFile;

typedef struct
{
    IoReader base;

    IoFile *file;
} IoFileReader;

typedef struct
{
    IoWriter base;

    IoFile *file;
} IoFileWriter;

MaybeError io_file_open(IoFile *self, Str path);

MaybeError io_file_create(IoFile *self, Str path);

IoFileReader io_file_read(IoFile *self);

IoFileWriter io_file_write(IoFile *self);

MaybeError io_file_close(IoFile *self);
