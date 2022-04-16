#pragma once

#include <brutal-gfx>
#include <embed/win-decl.h>

#include "event.h"

typedef struct _UiApp UiApp;
typedef struct _UiWin UiWin;

typedef enum
{
    UI_WIN_NORMAL,
    UI_WIN_MODAL,

    UI_WIN_DOCK_TOP,
    UI_WIN_DOCK_BOTTOM,
    UI_WIN_DOCK_LEFT,
    UI_WIN_DOCK_RIGHT,
} UiWinType;

struct _UiWin
{
    EmbedWin embed;
    UiWinType type;
    int refcount;
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

UiWin *ui_win_create(UiApp *app, MRectf bound, UiWinType type);

void ui_win_ref(UiWin *self);

void ui_win_deref(UiWin *self);

/* --- Properties ----------------------------------------------------------- */

void ui_win_hide(UiWin *self);

void ui_win_show(UiWin *self);

bool ui_win_visible(UiWin *self);

void ui_win_resize(UiWin *self, MRectf bound);

MRectf ui_win_bound(UiWin *self);

MRectf ui_win_content(UiWin *self);

void ui_win_mount(UiWin *self, struct _UiView *view);

/* --- Paint ---------------------------------------------------------------- */

GfxBuf ui_win_gfx(UiWin *self);

void ui_win_should_repaint(UiWin *self);

void ui_win_should_repaint_rect(UiWin *self, MRectf rect);

void ui_win_repaint(UiWin *self);

void ui_win_repaint_rect(UiWin *self, MRectf rect);

void ui_win_flip(UiWin *self, MRectf rect);

void ui_win_flip_full(UiWin *self);

/* --- Layout --------------------------------------------------------------- */

void ui_win_should_relayout(UiWin *self);

void ui_win_relayout(UiWin *self);

/* --- Events --------------------------------------------------------------- */

void ui_win_dispatch(UiWin *self, UiEvent *event);
