#pragma once

#include <brutal-base>

#if defined(SYSTEM_SDL)
#    include <embed/sdl/app.h>
#elif defined(SYSTEM_BRUTAL)
#    include <embed/brutal/app.h>
#else
#    error "Unsupported embedder!"
#endif

typedef struct _EmbedApp EmbedApp;
