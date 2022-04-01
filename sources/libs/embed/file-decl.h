#pragma once

#include <bruwutal/base.h>

#if defined(SYSTEM_EFI)
#    include <embed/efi/file.h>
#elif defined(SYSTEM_POSIX)
#    include <embed/posix/file.h>
#elif defined(SYSTEM_bruwutal)
#    include <embed/bruwutal/file.h>
#elif defined(SYSTEM_KERNEL)
#    include <embed/kernel/file.h>
#else
#    error "Unsupported embedder!"
#endif

typedef struct _EmbedFile EmbedFile;
