#pragma once

#include <brutal/base/defs.h>
#if defined(SYSTEM_EFI)
#    include <embed/efi/file.h>
#elif defined(SYSTEM_POSIX)
#    include <embed/posix/file.h>
#elif defined(SYSTEM_BRUTAL)
#    include <embed/brutal/file.h>
#elif defined(SYSTEM_KERNEL)
#    include <embed/brutal/file.h>
#else
#    error "Unsupported embedder!"
#endif

typedef struct _EmbedFile EmbedFile;
