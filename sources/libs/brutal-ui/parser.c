#include <brutal-debug>
#include <brutal-io>

#include "parser.h"

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

static UiLayoutKeyword from_string(Str str)
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

static UiLayoutKeyword next_keyword(Scan *scan)
{
    scan_begin(scan);
    scan_eat_match(scan, isalpha);
    return from_string(scan_end(scan));
}

static void eat_whitespace(Scan *scan)
{
    scan_eat_match(scan, isblank);
}

static MVec2f parse_vec(Scan *scan, MVec2f vec)
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

static MGravity parse_gravity(Scan *scan, MGravity gravity)
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

static MSpacing parse_spacing(Scan *scan, MSpacing spacing)
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

void ui_parse_layout(UiLayout *layout, Scan *scan)
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

void ui_parse_layout_str(UiLayout *layout, Str str)
{
    Scan scan;
    scan_init(&scan, str);
    ui_parse_layout(layout, &scan);
    assert_truth(!scan_dump_error(&scan, io_chan_out()));
}
