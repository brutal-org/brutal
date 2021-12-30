#pragma once

#include <brutal/text.h>

#if defined(SYSTEM_EFI)
#    include <embed/efi/file.h>
#elif defined(SYSTEM_POSIX)
#    include <embed/posix/file.h>
#elif defined(SYSTEM_BRUTAL)
#    include <embed/brutal/file.h>
#elif defined(SYSTEM_KERNEL)
#    include <embed/kernel/file.h>
#else
#    error "Unsupported embedder!"
#endif

MaybeError embed_file_open(EmbedFile *self, Str path);

MaybeError embed_file_create(EmbedFile *self, Str path);

MaybeError embed_file_close(EmbedFile *self);

IoResult embed_file_read(EmbedFile *self, uint8_t *data, size_t size);

IoResult embed_file_write(EmbedFile *self, uint8_t const *data, size_t size);
