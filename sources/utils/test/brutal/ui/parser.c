#include <brutal-test>
#include <brutal-ui>

static UiLayout try_parse_layout(char const *str)
{
    Scan scan;
    scan_init(&scan, str$(str));

    UiLayout layout = {};
    ui_parse_layout(&layout, &scan);
    expect$(!scan_dump_error(&scan, io_chan_err()));

    return layout;
}

#define TEST_CASE(STR, EXPECTED)                      \
    test_case$(STR ": " #EXPECTED)                    \
    {                                                 \
        assert_truth(try_parse_layout(STR) EXPECTED); \
    }

test$(ui_parse_layout_dock)
{
    TEST_CASE("dock", .type == UI_LAYOUT_DOCK);
    TEST_CASE("dock-start", .dock == M_DOCK_START);
    TEST_CASE("dock-s", .dock == M_DOCK_START);
    TEST_CASE("dock-end", .dock == M_DOCK_END);
    TEST_CASE("dock-e", .dock == M_DOCK_END);
    TEST_CASE("dock-top", .dock == M_DOCK_TOP);
    TEST_CASE("dock-t", .dock == M_DOCK_TOP);
    TEST_CASE("dock-bottom", .dock == M_DOCK_BOTTOM);
    TEST_CASE("dock-b", .dock == M_DOCK_BOTTOM);
    TEST_CASE("dock-fill", .dock == M_DOCK_FILL);
    TEST_CASE("dock-none", .dock == M_DOCK_NONE);
}

test$(ui_parse_layout_flex)
{
    TEST_CASE("flex", .type == UI_LAYOUT_FLEX);

    TEST_CASE("grow-3", .grow > 2.9);
    TEST_CASE("gaps-3", .gaps.x > 2.9);
    TEST_CASE("gaps-x-3", .gaps.x > 2.9);
    TEST_CASE("gaps-y-3", .gaps.y > 2.9);
}

test$(ui_parse_layout_grid)
{
    TEST_CASE("grid", .type == UI_LAYOUT_GRID);
}

test$(ui_parse_layout_min_max)
{
    TEST_CASE("min-width-3", .size.min.width > 2.9);
    TEST_CASE("min-height-3", .size.min.height > 2.9);
    TEST_CASE("max-width-3", .size.max.width > 2.9);
    TEST_CASE("max-height-3", .size.max.height > 2.9);

    TEST_CASE("min-w-3", .size.min.width > 2.9);
    TEST_CASE("min-h-3", .size.min.height > 2.9);
    TEST_CASE("max-w-3", .size.max.width > 2.9);
    TEST_CASE("max-h-3", .size.max.height > 2.9);

    TEST_CASE("min-x-3", .size.min.width > 2.9);
    TEST_CASE("min-y-3", .size.min.height > 2.9);
    TEST_CASE("max-x-3", .size.max.width > 2.9);
    TEST_CASE("max-y-3", .size.max.height > 2.9);
}

test$(ui_parse_layout_grivity)
{
    TEST_CASE("gravity-reset", .gravity == M_GRAVITY_NONE);
    TEST_CASE("gravity-s", .gravity == M_GRAVITY_START);
    TEST_CASE("gravity-start", .gravity == M_GRAVITY_START);
    TEST_CASE("gravity-e", .gravity == M_GRAVITY_END);
    TEST_CASE("gravity-end", .gravity == M_GRAVITY_END);
    TEST_CASE("gravity-t", .gravity == M_GRAVITY_TOP);
    TEST_CASE("gravity-top", .gravity == M_GRAVITY_TOP);
    TEST_CASE("gravity-b", .gravity == M_GRAVITY_BOTTOM);
    TEST_CASE("gravity-bottom", .gravity == M_GRAVITY_BOTTOM);
    TEST_CASE("gravity-center", .gravity == M_GRAVITY_CENTER);
    TEST_CASE("gravity-vcenter", .gravity == M_GRAVITY_VCENTER);
    TEST_CASE("gravity-hcenter", .gravity == M_GRAVITY_HCENTER);
    TEST_CASE("gravity-fill", .gravity == M_GRAVITY_FILL);
    TEST_CASE("gravity-vfill", .gravity == M_GRAVITY_VFILL);
    TEST_CASE("gravity-hfill", .gravity == M_GRAVITY_HFILL);

    TEST_CASE("g-reset", .gravity == M_GRAVITY_NONE);
    TEST_CASE("g-s", .gravity == M_GRAVITY_START);
    TEST_CASE("g-start", .gravity == M_GRAVITY_START);
    TEST_CASE("g-e", .gravity == M_GRAVITY_END);
    TEST_CASE("g-end", .gravity == M_GRAVITY_END);
    TEST_CASE("g-t", .gravity == M_GRAVITY_TOP);
    TEST_CASE("g-top", .gravity == M_GRAVITY_TOP);
    TEST_CASE("g-b", .gravity == M_GRAVITY_BOTTOM);
    TEST_CASE("g-bottom", .gravity == M_GRAVITY_BOTTOM);
    TEST_CASE("g-center", .gravity == M_GRAVITY_CENTER);
    TEST_CASE("g-vcenter", .gravity == M_GRAVITY_VCENTER);
    TEST_CASE("g-hcenter", .gravity == M_GRAVITY_HCENTER);
    TEST_CASE("g-fill", .gravity == M_GRAVITY_FILL);
    TEST_CASE("g-vfill", .gravity == M_GRAVITY_VFILL);
    TEST_CASE("g-hfill", .gravity == M_GRAVITY_HFILL);
}

test$(ui_parse_layout_placement)
{
    TEST_CASE("placement-reset", .placement == M_GRAVITY_NONE);
    TEST_CASE("placement-s", .placement == M_GRAVITY_START);
    TEST_CASE("placement-start", .placement == M_GRAVITY_START);
    TEST_CASE("placement-e", .placement == M_GRAVITY_END);
    TEST_CASE("placement-end", .placement == M_GRAVITY_END);
    TEST_CASE("placement-t", .placement == M_GRAVITY_TOP);
    TEST_CASE("placement-top", .placement == M_GRAVITY_TOP);
    TEST_CASE("placement-b", .placement == M_GRAVITY_BOTTOM);
    TEST_CASE("placement-bottom", .placement == M_GRAVITY_BOTTOM);
    TEST_CASE("placement-center", .placement == M_GRAVITY_CENTER);
    TEST_CASE("placement-vcenter", .placement == M_GRAVITY_VCENTER);
    TEST_CASE("placement-hcenter", .placement == M_GRAVITY_HCENTER);
    TEST_CASE("placement-fill", .placement == M_GRAVITY_FILL);
    TEST_CASE("placement-vfill", .placement == M_GRAVITY_VFILL);
    TEST_CASE("placement-hfill", .placement == M_GRAVITY_HFILL);

    TEST_CASE("place-reset", .placement == M_GRAVITY_NONE);
    TEST_CASE("place-s", .placement == M_GRAVITY_START);
    TEST_CASE("place-start", .placement == M_GRAVITY_START);
    TEST_CASE("place-e", .placement == M_GRAVITY_END);
    TEST_CASE("place-end", .placement == M_GRAVITY_END);
    TEST_CASE("place-t", .placement == M_GRAVITY_TOP);
    TEST_CASE("place-top", .placement == M_GRAVITY_TOP);
    TEST_CASE("place-b", .placement == M_GRAVITY_BOTTOM);
    TEST_CASE("place-bottom", .placement == M_GRAVITY_BOTTOM);
    TEST_CASE("place-center", .placement == M_GRAVITY_CENTER);
    TEST_CASE("place-vcenter", .placement == M_GRAVITY_VCENTER);
    TEST_CASE("place-hcenter", .placement == M_GRAVITY_HCENTER);
    TEST_CASE("place-fill", .placement == M_GRAVITY_FILL);
    TEST_CASE("place-vfill", .placement == M_GRAVITY_VFILL);
    TEST_CASE("place-hfill", .placement == M_GRAVITY_HFILL);
}

test$(ui_parse_layout_spacing)
{
    TEST_CASE("padding-3", .padding.start > 2.9);
    TEST_CASE("padding-3", .padding.end > 2.9);
    TEST_CASE("padding-3", .padding.top > 2.9);
    TEST_CASE("padding-3", .padding.bottom > 2.9);

    TEST_CASE("p-3", .padding.start > 2.9);
    TEST_CASE("p-3", .padding.end > 2.9);
    TEST_CASE("p-3", .padding.top > 2.9);
    TEST_CASE("p-3", .padding.bottom > 2.9);

    TEST_CASE("padding-start-3", .padding.start > 2.9);
    TEST_CASE("padding-end-3", .padding.end > 2.9);
    TEST_CASE("padding-top-3", .padding.top > 2.9);
    TEST_CASE("padding-bottom-3", .padding.bottom > 2.9);

    TEST_CASE("p-s-3", .padding.start > 2.9);
    TEST_CASE("p-e-3", .padding.end > 2.9);
    TEST_CASE("p-t-3", .padding.top > 2.9);
    TEST_CASE("p-b-3", .padding.bottom > 2.9);

    TEST_CASE("margin-3", .margin.start > 2.9);
    TEST_CASE("margin-3", .margin.end > 2.9);
    TEST_CASE("margin-3", .margin.top > 2.9);
    TEST_CASE("margin-3", .margin.bottom > 2.9);

    TEST_CASE("m-3", .margin.start > 2.9);
    TEST_CASE("m-3", .margin.end > 2.9);
    TEST_CASE("m-3", .margin.top > 2.9);
    TEST_CASE("m-3", .margin.bottom > 2.9);

    TEST_CASE("margin-start-3", .margin.start > 2.9);
    TEST_CASE("margin-end-3", .margin.end > 2.9);
    TEST_CASE("margin-top-3", .margin.top > 2.9);
    TEST_CASE("margin-bottom-3", .margin.bottom > 2.9);

    TEST_CASE("m-s-3", .margin.start > 2.9);
    TEST_CASE("m-e-3", .margin.end > 2.9);
    TEST_CASE("m-t-3", .margin.top > 2.9);
    TEST_CASE("m-b-3", .margin.bottom > 2.9);
}

test$(ui_parse_layout_flow)
{
    TEST_CASE("ltr", .flow == M_FLOW_LEFT_TO_RIGHT);
    TEST_CASE("rtl", .flow == M_FLOW_RIGHT_TO_LEFT);
    TEST_CASE("ttb", .flow == M_FLOW_TOP_TO_BOTTOM);
    TEST_CASE("btt", .flow == M_FLOW_BOTTOM_TO_TOP);
}
