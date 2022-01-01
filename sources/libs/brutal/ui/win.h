#pragma once

#include <brutal/gfx/gfx.h>
#include <brutal/ui/event.h>
#include <embed/win-decl.h>

typedef struct _UiApp UiApp;
typedef struct _UiWin UiWin;

enum
{
    UI_WIN_NIL = 0,

    UI_WIN_RESIZABLE = 1 << 0,
    UI_WIN_ANIMATED = 1 << 1,
};

struct _UiWin
{
    UiWinHandle handle;
    int refcount;

    int flags;
    EmbedWin embed;
    Gfx gfx;

    void (*deinit)(UiWin *self);
    void (*paint)(UiWin *self, Gfx *gfx);
    void (*event)(UiWin *self, UiEvent *event);

    bool grabbed;
    struct _UiView *overing;
    struct _UiView *focus;

    struct _UiApp *app;
    struct _UiView *root;
};

/* --- Lifecycle ------------------------------------------------------------ */

UiWin *ui_win_create(UiApp *app, MRect bound, int flags);

void ui_win_ref(UiWin *self);

void ui_win_deref(UiWin *self);

/* --- Properties ----------------------------------------------------------- */

void ui_win_hide(UiWin *self);

void ui_win_show(UiWin *self);

bool ui_win_visible(UiWin *self);

void ui_win_resize(UiWin *self, MRect bound);

MRect ui_win_bound(UiWin *self);

MRect ui_win_content(UiWin *self);

void ui_win_mount(UiWin *self, struct _UiView *view);

/* --- Paint ---------------------------------------------------------------- */

GfxBuf ui_win_gfx(UiWin *self);

void ui_win_should_repaint(UiWin *self);

void ui_win_should_repaint_rect(UiWin *self, MRect rect);

void ui_win_repaint(UiWin *self);

void ui_win_repaint_rect(UiWin *self, MRect rect);

void ui_win_flip(UiWin *self, MRect rect);

void ui_win_flip_full(UiWin *self);

/* --- Layout --------------------------------------------------------------- */

void ui_win_should_relayout(UiWin *self);

void ui_win_relayout(UiWin *self);

/* --- Events --------------------------------------------------------------- */

void ui_win_dispatch(UiWin *self, UiEvent *event);
