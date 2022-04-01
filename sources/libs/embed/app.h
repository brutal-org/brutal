#pragma once

#include <bruwutal/ui/app.h>

void embed_app_init(UiApp *self);

void embed_app_deinit(UiApp *self);

void embed_app_pump(UiApp *self);

void embed_app_wait(UiApp *self);
