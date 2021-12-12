#pragma once

#include <brutal/base/defs.h>

#if defined(SYSTEM_SDL)
#    include <embed/sdl/app.h>
#elif defined(SYSTEM_BRUTAL)
#    include <embed/brutal/app.h>
#else
#    error "Unsupported embedder!"
#endif

typedef struct embed_app EmbedApp;

void embed_app_init(EmbedApp *self);

void embed_app_deinit(EmbedApp *self);

void embed_app_pump(EmbedApp *self);
