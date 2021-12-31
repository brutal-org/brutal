#pragma once

#include <brutal/gfx/gfx.h>
#include <brutal/io/emit.h>
#include <brutal/ui/event.h>
#include <brutal/ui/style.h>

#define UI_VIEW_VISIBLE (1 << 0)
#define UI_VIEW_ENABLED (1 << 1)

typedef uint64_t UiFlags;

typedef struct _UiView UiView;
#define ui_view$(VIEW) ((UiView *)(VIEW))

struct _UiView
{
    int refcount;

    UiFlags flags;
    MRect bound;
    MVec2 scroll;
    UiStyle style;

    void (*deinit)(UiView *self);
    void (*paint)(UiView *self, Gfx *painter);
    void (*event)(UiView *self, UiEvent *event);
    MRect (*size)(UiView *self);
    void (*layout)(UiView *self);

    struct _UiApp *app;
    struct _UiWin *window;
    struct _UiView *parent;
    Vec(struct _UiView *) children;
};

/* --- Lifecycle ------------------------------------------------------------ */

#define ui_view_create(T) ({                                  \
    UiView *__view = (UiView *)alloc_make(alloc_global(), T); \
    ui_view_init(__view);                                     \
    __view;                                                   \
})

void ui_view_init(UiView *self);

void ui_view_ref(UiView *self);

void ui_view_deref(UiView *self);

void ui_view_dump(UiView *self, Emit *emit);

/* --- Childrens ------------------------------------------------------------ */

void ui_view_mount(UiView *self, UiView *child);

void ui_view_umount(UiView *self, UiView *child);

void ui_view_mounted_to_view(UiView *self, UiView *parent);

void ui_view_mounted_to_window(UiView *self, struct _UiWin *parent);

void ui_view_unmounted(UiView *self);

/* --- Properties ----------------------------------------------------------- */

MRect ui_view_bound(UiView *self);

MRect ui_view_content(UiView *self);

MRect ui_view_container(UiView *self);

void ui_view_resize(UiView *self, MRect rect);

void ui_view_style(UiView *self, UiStyle style);

struct _UiWin *ui_view_window(UiView *self);

/* --- Paint ---------------------------------------------------------------- */

void ui_view_should_repaint(UiView *self);

void ui_view_should_repaint_rect(UiView *self, MRect dirty);

void ui_view_paint(UiView *self, Gfx *gfx);

/* --- Layout --------------------------------------------------------------- */

void ui_view_should_relayout(UiView *self);

MRect ui_view_size(UiView *self, MRect parent);

void ui_view_place(UiView *self, MRect container);

void ui_view_layout(UiView *self);

/* --- Events --------------------------------------------------------------- */

void ui_view_event(UiView *self, UiEvent *event);
