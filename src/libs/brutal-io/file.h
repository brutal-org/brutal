#pragma once

#ifndef __kernel__

#    include <embed/file-decl.h>

#    include "traits.h"

typedef struct
{
    EmbedFile embed;
} IoFile;

typedef enum
{
    IO_FILE_READ = (1 << 0),
    IO_FILE_WRITE = (1 << 1),
    IO_FILE_READ_WRITE = (IO_FILE_READ | IO_FILE_WRITE),
    IO_FILE_CREATE = (1 << 2),
} FileInitFlags;

/**
 * @brief Open a file in **read-only**.
 *
 * @param self The file to open.
 * @param path The path of the file.
 * @return MaybeError
 */
MaybeError io_file_view(IoFile *self, Str path);

/**
 * @brief Create/Open a file in **write-only** .
 *
 * @param self The file to open.
 * @param path The path of the file.
 * @return MaybeError
 */
MaybeError io_file_create(IoFile *self, Str path);

/**
 * @brief Open a file in **read-write**.
 *
 * @param self The file to open.
 * @param path The path of the file
 * @return MaybeError
 */
MaybeError io_file_edit(IoFile *self, Str path);

/**
 * @brief Initialize a file object with specific flags.
 *
 * @param self The file to initialize.
 * @param path The path of the file.
 * @param flags Flags to use when loading the file.
 * @return MaybeError
 */
MaybeError io_file_init(IoFile *self, Str path, FileInitFlags flags);

IoReader io_file_reader(IoFile *self);

IoWriter io_file_writer(IoFile *self);

IoDuplex io_file_duplex(IoFile *self);

IoSeeker io_file_seeker(IoFile *self);

IoRSeek io_file_rseek(IoFile *self);

IoWSeek io_file_wseek(IoFile *self);

MaybeError io_file_close(IoFile *self);

#endif
