#pragma once

#include "color.h"
#include "event.h"
#include "layout.h"

#define UI_VIEW_VISIBLE (1 << 0)
#define UI_VIEW_ENABLED (1 << 1)
#define UI_VIEW_GREEDY (1 << 2)

typedef uint64_t UiFlags;

typedef struct _UiView UiView;
#define ui_view$(VIEW) ((UiView *)(VIEW))

struct _UiView
{
    int refcount;

    UiFlags flags;
    MRectf bound;
    MVec2f scroll;

    UiLayout layout;
    UiPalette palette;

    void (*deinit)(UiView *self);
    void (*repaint)(UiView *self, Gfx *painter);
    void (*event)(UiView *self, UiEvent *event);
    MRectf (*size)(UiView *self);
    void (*relayout)(UiView *self);

    struct _UiApp *app;
    struct _UiWin *window;
    struct _UiView *parent;
    Vec(struct _UiView *) children;
};

/* --- Lifecycle ------------------------------------------------------------ */

#define ui_view_create$(T) ({                                 \
    UiView *__view = (UiView *)alloc_make(alloc_global(), T); \
    ui_view_init(__view);                                     \
    __view;                                                   \
})

void ui_view_init(UiView *self);

void ui_view_ref(UiView *self);

void ui_view_deref(UiView *self);

/* --- Childrens ------------------------------------------------------------ */

void ui_view_mount(UiView *self, UiView *child);

void ui_view_umount(UiView *self, UiView *child);

void ui_view_mounted_to_view(UiView *self, UiView *parent);

void ui_view_mounted_to_window(UiView *self, struct _UiWin *parent);

void ui_view_unmounted(UiView *self);

/* --- Properties ----------------------------------------------------------- */

MVec2f ui_view_orgin(UiView *self);

MRectf ui_view_bound(UiView *self);

MRectf ui_view_content(UiView *self);

MRectf ui_view_container(UiView *self);

void ui_view_resize(UiView *self, MRectf rect);

void ui_view_layout(UiView *self, char const *layout);

GfxColor ui_view_color(UiView *self, UiRole role);

void ui_view_overide_color(UiView *self, UiRole role, GfxColor color);

struct _UiWin *ui_view_window(UiView *self);

/* --- Paint ---------------------------------------------------------------- */

void ui_view_should_repaint(UiView *self);

void ui_view_should_repaint_rect(UiView *self, MRectf dirty);

void ui_view_repaint(UiView *self, Gfx *gfx);

/* --- Layout --------------------------------------------------------------- */

void ui_view_should_relayout(UiView *self);

MRectf ui_view_size(UiView *self);

void ui_view_place(UiView *self, MRectf container);

void ui_view_relayout(UiView *self);

/* --- Events --------------------------------------------------------------- */

UiView *ui_view_lookup(UiView *self, MVec2f pos);

void ui_view_dispatch(UiView *self, UiEvent *event);
