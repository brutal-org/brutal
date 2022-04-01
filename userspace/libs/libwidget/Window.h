#pragma once

#include <libasync/Invoker.h>
#include <libgraphic/Bitmap.h>
#include <libgraphic/Painter.h>
#include <libio/Streams.h>
#include <libutils/HashMap.h>
#include <libutils/Vec.h>
#include <libwidget/Cursor.h>
#include <libwidget/Element.h>
#include <libwidget/Event.h>

#include "compositor/Protocol.h"

#define WINDOW_RESIZE_AREA 16

namespace Widget
{

struct Window
{
private:
    int _handle = -1;

    Math::Recti _bound{250, 250};
    WindowFlag _flags;
    WindowType _type = WINDOW_TYPE_REGULAR;

    float _opacity;

    bool _focused = false;
    bool _visible = false;

    bool _is_maximised = false;
    bool _is_resizing = false;

    bool _resize_vertical = false;
    bool _resize_horizontal = false;

    Math::Vec2i _resize_begin;
    Math::Recti _previous_bound;

    CursorState cursor_state = CURSOR_DEFAULT;

    Ref<Graphic::Bitmap> frontbuffer;
    Ref<Graphic::Bitmap> backbuffer;

    bool _dirty_layout;
    Vec<Math::Recti> _dirty_paint{};

    EventHandler _handlers[EventType::__COUNT];

    Ref<Element> _root;

    Element *_keyboard_focus = nullptr;
    Element *_mouse_focus = nullptr;
    Element *_mouse_over = nullptr;

    Box<Async::Invoker> _update_invoker;

    NONCOPYABLE(Window);
    NONMOVABLE(Window);

public:
    int handle() { return this->_handle; }

    int frontbuffer_handle() const { return frontbuffer->handle(); }

    Math::Vec2i frontbuffer_size() const { return frontbuffer->size(); }

    int backbuffer_handle() const { return backbuffer->handle(); }

    Math::Vec2i backbuffer_size() const { return backbuffer->size(); }

    WindowFlag flags() { return _flags; }

    void opacity(float value) { _opacity = value; }

    float opacity() { return _opacity; }

    bool visible() { return _visible; }

    bool focused()
    {
        return (_flags & WINDOW_ALWAYS_FOCUSED) || _focused;
    }

    bool maximised() { return _is_maximised; }

    void toggle_maximise();

    WindowType type() { return _type; }

    void type(WindowType type) { _type = type; }

    Graphic::Color color(ThemeColorRole role);

    Window(WindowFlag flags = WINDOW_NONE);

    virtual ~Window();

    void show();

    void hide();

    void try_hide();

    void cursor(CursorState state);

    /* --- Geometry --------------------------------------------------------- */

    Math::Vec2i position() { return bound_on_screen().position(); }

    void position(Math::Vec2i position) { bound(bound_on_screen().moved(position)); }

    Math::Vec2i size() { return bound().size(); }

    void size(Math::Vec2i size) { bound(bound_on_screen().resized(size)); }

    void resize_to_content();

    Math::Recti bound() { return _bound.moved({0, 0}); }

    void bound(Math::Recti bound);

    Math::Recti bound_on_screen() { return _bound; }

    void change_framebuffer_if_needed();

    Math::Border resize_bound_containe(Math::Vec2i position);

    void begin_resize(Math::Vec2i mouse_position);

    void do_resize(Math::Vec2i mouse_position);

    void end_resize();

    /* --- Children --------------------------------------------------------- */

    Ref<Element> root()
    {
        if (!_root)
        {
            _root = build();
            _root->mount(*this);
            _keyboard_focus = _root.naked();
        }

        return _root;
    }

    void focus_widget(Element *widget);

    void widget_removed(Element *widget);

    Element *child_at(Math::Vec2i position);

    /* --- Focus ------------------------------------------------------------ */

    bool has_keyboard_focus(Element *widget);

    /* --- Rebuild ---------------------------------------------------------- */

    virtual Ref<Element> build()
    {
        return make<Element>();
    }

    void should_rebuild()
    {
        if (_root)
        {
            _root->unmount();
            _root = nullptr;
            should_relayout();
        }
    }

    /* --- Update ----------------------------------------------------------- */

    void relayout();

    virtual void repaint(Graphic::Painter &painter, Math::Recti rectangle);

    void flip(Math::Recti region);

    void update();

    void should_relayout();

    void should_repaint(Math::Recti rectangle);

    /* --- Events ----------------------------------------------------------- */

    void on(EventType event, EventHandler handler);

    void dispatch_event(Event *event);

    void handle_event(Event *Event);

    void handle_got_focus(Event *event);

    void handle_lost_focus(Event *event);

    void handle_mouse_move(Event *event);

    void handle_mouse_scroll(Event *event);

    void handle_mouse_button_press(Event *event);

    void handle_mouse_button_release(Event *event);

    void handle_mouse_double_click(Event *event);

    void handle_keyboard_key_typed(Event *event);

    void handle_keyboard_key_press(Event *event);

    void handle_keyboard_key_release(Event *event);
};

static inline Box<Window> window(WindowFlag flags, Math::Vec2i size, Ref<Element> root)
{
    auto window = own<Window>(flags);
    window->size(size);
    window->root()->add(root);
    return window;
}

static inline Box<Window> window(WindowFlag flags, Ref<Element> root)
{
    auto window = own<Window>(flags);
    window->root()->add(root);
    window->resize_to_content();
    return window;
}

static inline Box<Window> window(Ref<Element> root)
{
    return window(WINDOW_NONE, root);
}

} // namespace Widget
