#pragma once

#if defined(__linux__)
#    include <host/linux/app.h>
#elif defined(__brutal__)
#    include <host/brutal/app.h>
#else
#    error "Unsupported platform!"
#endif

typedef struct host_app HostApp;

void host_app_init(HostApp *self);

void host_app_deinit(HostApp *self);

void host_app_pump(HostApp *self);
