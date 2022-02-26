#include <brutal/io/std.h>
#include <brutal/parse/nums.h>
#include <brutal/ui/layout.h>
#include <brutal/ui/view.h>

/* --- Compute Size --------------------------------------------------------- */

static MVec2 ui_dock_size(UiView *views[], size_t len)
{
    MVec2 total_size = m_vec2(0, 0);
    MVec2 current_size = m_vec2(0, 0);
    MOrientation previous_orientation = M_ORIENTATION_NONE;

    for (size_t i = 0; i < len; i++)
    {
        UiLayout const *child_layout = &views[i]->layout;
        MRect child_bound = ui_view_size(views[i]);
        MOrientation child_orientation = m_dock_orientation(child_layout->dock);

        if (previous_orientation != child_orientation)
        {
            total_size = m_vec2_add(total_size, current_size);

            previous_orientation = child_orientation;
            current_size = m_vec2(0, 0);
        }

        switch (child_orientation)
        {
        case M_ORIENTATION_NONE:
            current_size = m_vec2_max(current_size, child_bound.size);
            break;

        case M_ORIENTATION_HORIZONTAL:
            current_size.x += child_bound.size.x;
            current_size.y = m_max(current_size.y, child_bound.size.y);
            break;

        case M_ORIENTATION_VERTICAL:
            current_size.x = m_max(current_size.x, child_bound.size.x);
            current_size.y += child_bound.size.y;
            break;

        default:
            panic$("Invalid orientation");
        }
    }

    total_size = m_vec2_add(total_size, current_size);

    return total_size;
}

static MVec2 ui_flex_size(UiLayout const *self, UiView *views[], size_t len)
{
    MVec2 size = m_vec2(0, 0);
    MFlow flow = self->flow;

    for (size_t i = 0; i < len; i++)
    {
        UiLayout const *child_layout = &views[i]->layout;
        MRect child_bound = ui_view_size(views[i]);

        if (child_layout->dock == M_DOCK_START ||
            child_layout->dock == M_DOCK_END ||
            child_layout->dock == M_DOCK_FILL)
        {
            size.x += m_flow_get_width(flow, child_bound);
        }

        if (child_layout->dock == M_DOCK_TOP ||
            child_layout->dock == M_DOCK_BOTTOM ||
            child_layout->dock == M_DOCK_FILL)
        {
            size.y = m_max(m_flow_get_height(flow, child_bound), size.y);
        }
    }

    size.y += self->gaps.x * (len - 1);

    return size;
}

static MVec2 ui_max_size(struct _UiView *views[], size_t len)
{
    MVec2 result = m_vec2(0, 0);

    for (size_t i = 0; i < len; i++)
    {
        MVec2 size = ui_view_size(views[i]).size;
        result.x = m_max(result.x, size.x);
        result.y = m_max(result.y, size.y);
    }

    return result;
}

MVec2 ui_layout_size(UiLayout const *self, struct _UiView *views[], size_t len)
{
    switch (self->type)
    {
    case UI_LAYOUT_DOCK:
        return ui_dock_size(views, len);

    case UI_LAYOUT_FLEX:
        return ui_flex_size(self, views, len);

    default:
        return ui_max_size(views, len);
    }
}

/* --- Layout --------------------------------------------------------------- */

static void ui_dock_run(UiLayout const *self, MRect container, UiView *views[], size_t len)
{
    for (size_t i = 0; i < len; i++)
    {
        UiView *child = views[i];
        MRect content = ui_view_size(child);
        content = m_dock_apply(child->layout.dock, self->flow, content, &container);
        ui_view_place(child, content);
    }
}

static void ui_flex_run(UiLayout const *self, MRect container, UiView *views[], size_t len)
{
    MFlow flow = self->flow;

    float grows = 0;
    float total = 0;

    for (size_t i = 0; i < len; i++)
    {
        UiView *child = views[i];
        UiLayout child_layout = child->layout;

        if (child_layout.grow > 0.01)
        {
            grows += child_layout.grow;
        }
        else
        {
            total += m_flow_get_width(flow, ui_view_size(child));
        }
    }

    float all = m_flow_get_width(flow, container) - self->gaps.x * (len - 1);
    float grow_total = m_max(0, all - total);
    float grow_unit = (grow_total) / m_max(1, grows);
    float start = m_flow_get_start(flow, container);

    for (size_t i = 0; i < len; i++)
    {
        UiView *child = views[i];
        UiLayout child_layout = child->layout;
        MRect child_bound = {};

        if (child_layout.grow > 0.01)
        {
            child_bound = m_flow_set_start(flow, child_bound, start);
            child_bound = m_flow_set_width(flow, child_bound, grow_unit * child_layout.grow);
        }
        else
        {
            child_bound = ui_view_size(child);
            child_bound = m_flow_set_x(flow, child_bound, start);
        }

        child_bound = m_flow_set_y(flow, child_bound, m_flow_get_top(flow, container));
        child_bound = m_flow_set_height(flow, child_bound, m_flow_get_height(flow, container));

        ui_view_place(child, child_bound);
        start += m_flow_get_width(flow, child_bound) + self->gaps.x;
    }
}

void ui_layout_run(UiLayout const *self, MRect container, UiView *views[], size_t len)
{
    switch (self->type)
    {
    case UI_LAYOUT_DOCK:
        ui_dock_run(self, container, views, len);
        break;

    case UI_LAYOUT_FLEX:
        ui_flex_run(self, container, views, len);
        break;

    default:
        break;
    }
}

/* --- Parser --------------------------------------------------------------- */

#define FOR_EACH_KEYWORDS(KEYWORD) \
    KEYWORD(UNKNOW)                \
    KEYWORD(NONE)                  \
    KEYWORD(RESET)                 \
                                   \
    KEYWORD(DOCK)                  \
    KEYWORD(FLEX)                  \
    KEYWORD(GRID)                  \
                                   \
    KEYWORD(G)                     \
    KEYWORD(GRAVITY)               \
    KEYWORD(PLACE)                 \
    KEYWORD(PLACEMENT)             \
    KEYWORD(VERTICAL)              \
    KEYWORD(HORIZONTAL)            \
    KEYWORD(MIN)                   \
    KEYWORD(MAX)                   \
    KEYWORD(W)                     \
    KEYWORD(WIDTH)                 \
    KEYWORD(H)                     \
    KEYWORD(HEIGHT)                \
    KEYWORD(X)                     \
    KEYWORD(Y)                     \
                                   \
    KEYWORD(FILL)                  \
    KEYWORD(HFILL)                 \
    KEYWORD(VFILL)                 \
    KEYWORD(S)                     \
    KEYWORD(START)                 \
    KEYWORD(T)                     \
    KEYWORD(TOP)                   \
    KEYWORD(E)                     \
    KEYWORD(END)                   \
    KEYWORD(B)                     \
    KEYWORD(BOTTOM)                \
    KEYWORD(COVER)                 \
    KEYWORD(FIT)                   \
    KEYWORD(CENTER)                \
    KEYWORD(HCENTER)               \
    KEYWORD(VCENTER)               \
    KEYWORD(ALL)                   \
                                   \
    KEYWORD(P)                     \
    KEYWORD(PADDING)               \
    KEYWORD(M)                     \
    KEYWORD(MARGIN)                \
                                   \
    KEYWORD(LTR)                   \
    KEYWORD(RTL)                   \
    KEYWORD(TTB)                   \
    KEYWORD(BTT)                   \
    KEYWORD(GAPS)                  \
    KEYWORD(GROW)

typedef enum
{
#define ITER(NAME) UI_KW_##NAME,
    FOR_EACH_KEYWORDS(ITER)
#undef ITER
} UiLayoutKeyword;

static const char *ui_layout_keyword_names[] = {
#define ITER(NAME) #NAME,
    FOR_EACH_KEYWORDS(ITER)
#undef ITER
};

UiLayoutKeyword from_string(Str str)
{
    for (size_t i = 0; i < sizeof(ui_layout_keyword_names) / sizeof(const char *); i++)
    {
        if (str_eq_ci(str, str$(ui_layout_keyword_names[i])))
        {
            return (UiLayoutKeyword)i;
        }
    }

    return UI_KW_UNKNOW;
}

UiLayoutKeyword next_keyword(Scan *scan)
{
    scan_begin(scan);
    scan_skip_until(scan, isalpha);
    return from_string(scan_end(scan));
}

void eat_whitespace(Scan *scan)
{
    scan_skip_until(scan, isblank);
}

MVec2 parse_vec(Scan *scan, MVec2 vec)
{
    switch (next_keyword(scan))
    {
    case UI_KW_X:
    case UI_KW_W:
    case UI_KW_WIDTH:
        scan_expect(scan, '-');
        scan_next_float(scan, &vec.width);
        break;

    case UI_KW_Y:
    case UI_KW_H:
    case UI_KW_HEIGHT:
        scan_expect(scan, '-');
        scan_next_float(scan, &vec.height);
        break;

    default:
        scan_throw(scan, str$("Expected width or height"), nullstr);
    }

    return vec;
}

MGravity parse_gravity(Scan *scan, MGravity gravity)
{
    switch (next_keyword(scan))
    {
    case UI_KW_RESET:
        gravity = M_GRAVITY_NONE;
        break;

    case UI_KW_S:
    case UI_KW_START:
        gravity |= M_GRAVITY_START;
        break;

    case UI_KW_E:
    case UI_KW_END:
        gravity |= M_GRAVITY_END;
        break;

    case UI_KW_T:
    case UI_KW_TOP:
        gravity |= M_GRAVITY_TOP;
        break;

    case UI_KW_B:
    case UI_KW_BOTTOM:
        gravity |= M_GRAVITY_BOTTOM;
        break;

    case UI_KW_CENTER:
        gravity |= M_GRAVITY_CENTER;
        break;

    case UI_KW_VCENTER:
        gravity |= M_GRAVITY_VCENTER;
        break;

    case UI_KW_HCENTER:
        gravity |= M_GRAVITY_HCENTER;
        break;

    case UI_KW_FILL:
        gravity |= M_GRAVITY_FILL;
        break;

    case UI_KW_VFILL:
        gravity |= M_GRAVITY_VFILL;
        break;

    case UI_KW_HFILL:
        gravity |= M_GRAVITY_HFILL;
        break;

    default:
        scan_throw(scan, str$("Expected gravity"), nullstr);
        break;
    }

    return gravity;
}

MSpacing parse_spacing(Scan *scan, MSpacing spacing)
{
    if (isdigit(scan_curr(scan)))
    {
        scan_next_float(scan, &spacing.start);
        spacing.end = spacing.start;
        spacing.top = spacing.start;
        spacing.bottom = spacing.start;
        return spacing;
    }

    switch (next_keyword(scan))
    {
    case UI_KW_RESET:
        spacing = (MSpacing){};
        break;

    case UI_KW_S:
    case UI_KW_START:
        scan_expect(scan, '-');
        scan_next_float(scan, &spacing.start);
        break;

    case UI_KW_E:
    case UI_KW_END:
        scan_expect(scan, '-');
        scan_next_float(scan, &spacing.end);
        break;

    case UI_KW_T:
    case UI_KW_TOP:
        scan_expect(scan, '-');
        scan_next_float(scan, &spacing.top);
        break;

    case UI_KW_B:
    case UI_KW_BOTTOM:
        scan_expect(scan, '-');
        scan_next_float(scan, &spacing.bottom);
        break;

    case UI_KW_X:
        scan_expect(scan, '-');
        scan_next_float(scan, &spacing.start);
        spacing.end = spacing.start;
        break;

    case UI_KW_Y:
        scan_expect(scan, '-');
        scan_next_float(scan, &spacing.top);
        spacing.bottom = spacing.top;
        break;

    case UI_KW_ALL:
        scan_expect(scan, '-');
        scan_next_float(scan, &spacing.start);
        spacing.end = spacing.start;
        spacing.top = spacing.start;
        spacing.bottom = spacing.start;
        break;

    default:
        scan_throw(scan, str$("Expected spacing"), nullstr);
        break;
    }

    return spacing;
}

void ui_layout_parse(UiLayout *layout, Scan *scan)
{
    eat_whitespace(scan);

    while (!scan_ended(scan))
    {
        switch (next_keyword(scan))
        {
        case UI_KW_RESET:
            *layout = (UiLayout){};
            break;

        case UI_KW_DOCK:
            if (scan_skip(scan, '-'))
            {
                switch (next_keyword(scan))
                {
                case UI_KW_NONE:
                    layout->dock = M_DOCK_NONE;
                    break;

                case UI_KW_FILL:
                    layout->dock = M_DOCK_FILL;
                    break;

                case UI_KW_S:
                case UI_KW_START:
                    layout->dock = M_DOCK_START;
                    break;

                case UI_KW_E:
                case UI_KW_END:
                    layout->dock = M_DOCK_END;
                    break;

                case UI_KW_T:
                case UI_KW_TOP:
                    layout->dock = M_DOCK_TOP;
                    break;

                case UI_KW_B:
                case UI_KW_BOTTOM:
                    layout->dock = M_DOCK_BOTTOM;
                    break;

                default:
                    scan_throw(scan, str$("Expected dock"), nullstr);
                    break;
                }
            }
            else
            {
                layout->type = UI_LAYOUT_DOCK;
            }
            break;

        case UI_KW_FLEX:
            layout->type = UI_LAYOUT_FLEX;
            break;

        case UI_KW_GRID:
            layout->type = UI_LAYOUT_GRID;
            break;

        case UI_KW_MIN:
            scan_expect(scan, '-');
            layout->size.min = parse_vec(scan, layout->size.min);
            break;

        case UI_KW_MAX:
            scan_expect(scan, '-');
            layout->size.max = parse_vec(scan, layout->size.max);
            break;

        case UI_KW_G:
        case UI_KW_GRAVITY:
            scan_expect(scan, '-');
            layout->gravity = parse_gravity(scan, layout->gravity);
            break;

        case UI_KW_PLACE:
        case UI_KW_PLACEMENT:
            scan_expect(scan, '-');
            layout->placement = parse_gravity(scan, layout->placement);
            break;

        case UI_KW_P:
        case UI_KW_PADDING:
            scan_expect(scan, '-');
            layout->padding = parse_spacing(scan, layout->padding);
            break;

        case UI_KW_M:
        case UI_KW_MARGIN:
            scan_expect(scan, '-');
            layout->margin = parse_spacing(scan, layout->margin);
            break;

        case UI_KW_LTR:
            layout->flow = M_FLOW_LEFT_TO_RIGHT;
            break;

        case UI_KW_RTL:
            layout->flow = M_FLOW_RIGHT_TO_LEFT;
            break;

        case UI_KW_TTB:
            layout->flow = M_FLOW_TOP_TO_BOTTOM;
            break;

        case UI_KW_BTT:
            layout->flow = M_FLOW_BOTTOM_TO_TOP;
            break;

        case UI_KW_GROW:
            scan_expect(scan, '-');
            scan_next_float(scan, &layout->grow);
            break;

        case UI_KW_GAPS:
            scan_expect(scan, '-');
            if (isdigit(scan_curr(scan)))
            {
                scan_next_float(scan, &layout->gaps.x);
            }
            else
            {
                layout->gaps = parse_vec(scan, layout->gaps);
            }

            break;

        default:
            scan_throw(scan, str$("Unexpected keyword"), nullstr);
            break;
        }

        eat_whitespace(scan);
    }
}

void ui_layout_parse_str(UiLayout *layout, Str str)
{
    Scan scan;
    scan_init(&scan, str);
    ui_layout_parse(layout, &scan);
    assert_truth(!scan_dump_error(&scan, io_chan_out()));
}
