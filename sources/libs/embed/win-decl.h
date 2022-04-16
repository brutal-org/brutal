#pragma once

#include <brutal-base>

#if defined(SYSTEM_SDL)
#    include <embed/sdl/win.h>
#elif defined(SYSTEM_BRUTAL)
#    include <embed/brutal/win.h>
#else
#    error "Unsupported embedder!"
#endif

typedef struct _EmbedWin EmbedWin;
