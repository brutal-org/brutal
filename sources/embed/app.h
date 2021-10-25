#pragma once

#include <brutal/base/defs.h>

#if defined(SYSTEM_SDL)
#    include <embed/sdl/app.h>
#elif defined(SYSTEM_BRUTAL)
#    include <embed/brutal/app.h>
#else
#    error "Unsupported embedder!"
#endif

typedef struct host_app HostApp;

void host_app_init(HostApp *self);

void host_app_deinit(HostApp *self);

void host_app_pump(HostApp *self);
