#pragma once

#include <bruwutal/base.h>

#if defined(SYSTEM_SDL)
#    include <embed/sdl/app.h>
#elif defined(SYSTEM_bruwutal)
#    include <embed/bruwutal/app.h>
#else
#    error "Unsupported embedder!"
#endif

typedef struct _EmbedApp EmbedApp;
