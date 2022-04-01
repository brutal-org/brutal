#pragma once

#include <bruwutal/base.h>

#if defined(SYSTEM_SDL)
#    include <embed/sdl/win.h>
#elif defined(SYSTEM_bruwutal)
#    include <embed/bruwutal/win.h>
#else
#    error "Unsupported embedder!"
#endif

typedef struct _EmbedWin EmbedWin;
