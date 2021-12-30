#pragma once

#include <brutal/base/defs.h>

#if defined(SYSTEM_SDL)
#    include <embed/sdl/win.h>
#elif defined(SYSTEM_BRUTAL)
#    include <embed/brutal/win.h>
#else
#    error "Unsupported embedder!"
#endif

typedef struct embed_win EmbedWin;
