#pragma once

#include <brutal/alloc/base.h>
#include <brutal/gfx/buf.h>
#include <brutal/gfx/gfx.h>
#include <brutal/ui/app.h>
#include <brutal/ui/event.h>
#include <embed/win-decl.h>

typedef struct ui_app UiApp;
typedef struct ui_body UiBody;

enum
{
    UI_WIN_NIL = 0,

    UI_WIN_RESIZABLE = 1 << 0,
    UI_WIN_ANIMATED = 1 << 1,
};

#define UI_WIN_FIELDS(SELF)                      \
    int flags;                                   \
    EmbedWin embed;                              \
    UiApp *app;                                  \
    Gfx gfx;                                     \
                                                 \
    void (*paint)(SELF * self, Gfx * painter);   \
    void (*event)(SELF * self, UiEvent * event); \
    void (*deinit)(SELF * self);

typedef struct ui_win
{
    UI_WIN_FIELDS(struct ui_win);
} UiWin;

#define UI_WIN_BASE(SELF)       \
    union                       \
    {                           \
        UiWin base;             \
        struct                  \
        {                       \
            UI_WIN_FIELDS(SELF) \
        };                      \
    }

/* --- Lifecycle ------------------------------------------------------------ */

void ui_win_init(UiWin *win, UiApp *app, Recti bound, int flags);

void ui_win_destroy(UiWin *self);

/* --- Properties --------------------------------------------------------------- */

void ui_win_hide(UiWin *self);

void ui_win_show(UiWin *self);

bool ui_win_visible(UiWin *self);

void ui_win_resize(UiWin *self, Recti bound);

Recti ui_win_bound(UiWin *self);

/* --- Paint ---------------------------------------------------------------- */

GfxBuf ui_win_gfx(UiWin *self);

void ui_win_repaint(UiWin *self);

void ui_win_flip(UiWin *self, Recti rect);

void ui_win_flip_full(UiWin *self);

/* --- Events --------------------------------------------------------------- */

void ui_win_dispatch(UiWin *self, UiEvent *event);
