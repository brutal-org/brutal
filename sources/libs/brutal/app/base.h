#pragma once

#include <brutal/base/std.h>
#include <host/app.h>

typedef struct
{
    HostApp host;

    bool running;
    int exit_value;
} App;

void app_init(App *self);

void app_deinit(App *self);

int app_run(App *self);

void app_pump(App *self);

void app_exit(App *self, int exit);
