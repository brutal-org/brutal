#include <brutal/ui.h>
#include "test/test.h"

static UiLayout try_parse_layout(char const *str)
{
    Scan scan;
    scan_init(&scan, str$(str));

    UiLayout layout = {};
    ui_parse_layout(&layout, &scan);
    assert_truth(!scan_dump_error(&scan, io_chan_err()));

    return layout;
}

TEST(ui_parse_layout_dock)
{
    assert_truth(try_parse_layout("dock").type == UI_LAYOUT_DOCK);

    assert_truth(try_parse_layout("dock-start").dock == M_DOCK_START);
    assert_truth(try_parse_layout("dock-s").dock == M_DOCK_START);
    assert_truth(try_parse_layout("dock-end").dock == M_DOCK_END);
    assert_truth(try_parse_layout("dock-e").dock == M_DOCK_END);
    assert_truth(try_parse_layout("dock-top").dock == M_DOCK_TOP);
    assert_truth(try_parse_layout("dock-t").dock == M_DOCK_TOP);
    assert_truth(try_parse_layout("dock-bottom").dock == M_DOCK_BOTTOM);
    assert_truth(try_parse_layout("dock-b").dock == M_DOCK_BOTTOM);
    assert_truth(try_parse_layout("dock-fill").dock == M_DOCK_FILL);
    assert_truth(try_parse_layout("dock-none").dock == M_DOCK_NONE);
}

TEST(ui_parse_layout_flex)
{
    assert_truth(try_parse_layout("flex").type == UI_LAYOUT_FLEX);

    assert_truth(try_parse_layout("grow-3").grow > 2.9);
    assert_truth(try_parse_layout("gaps-3").gaps.x > 2.9);
    assert_truth(try_parse_layout("gaps-x-3").gaps.x > 2.9);
    assert_truth(try_parse_layout("gaps-y-3").gaps.y > 2.9);
}

TEST(ui_parse_layout_grid)
{
    assert_truth(try_parse_layout("grid").type == UI_LAYOUT_GRID);
}

TEST(ui_parse_layout_min_max)
{
    assert_truth(try_parse_layout("min-width-3").size.min.width > 2.9);
    assert_truth(try_parse_layout("min-height-3").size.min.height > 2.9);
    assert_truth(try_parse_layout("max-width-3").size.max.width > 2.9);
    assert_truth(try_parse_layout("max-height-3").size.max.height > 2.9);

    assert_truth(try_parse_layout("min-w-3").size.min.width > 2.9);
    assert_truth(try_parse_layout("min-h-3").size.min.height > 2.9);
    assert_truth(try_parse_layout("max-w-3").size.max.width > 2.9);
    assert_truth(try_parse_layout("max-h-3").size.max.height > 2.9);

    assert_truth(try_parse_layout("min-x-3").size.min.width > 2.9);
    assert_truth(try_parse_layout("min-y-3").size.min.height > 2.9);
    assert_truth(try_parse_layout("max-x-3").size.max.width > 2.9);
    assert_truth(try_parse_layout("max-y-3").size.max.height > 2.9);
}

TEST(ui_parse_layout_grivity)
{
    assert_truth(try_parse_layout("gravity-reset").gravity == M_GRAVITY_NONE);
    assert_truth(try_parse_layout("gravity-s").gravity == M_GRAVITY_START);
    assert_truth(try_parse_layout("gravity-start").gravity == M_GRAVITY_START);
    assert_truth(try_parse_layout("gravity-e").gravity == M_GRAVITY_END);
    assert_truth(try_parse_layout("gravity-end").gravity == M_GRAVITY_END);
    assert_truth(try_parse_layout("gravity-t").gravity == M_GRAVITY_TOP);
    assert_truth(try_parse_layout("gravity-top").gravity == M_GRAVITY_TOP);
    assert_truth(try_parse_layout("gravity-b").gravity == M_GRAVITY_BOTTOM);
    assert_truth(try_parse_layout("gravity-bottom").gravity == M_GRAVITY_BOTTOM);
    assert_truth(try_parse_layout("gravity-center").gravity == M_GRAVITY_CENTER);
    assert_truth(try_parse_layout("gravity-vcenter").gravity == M_GRAVITY_VCENTER);
    assert_truth(try_parse_layout("gravity-hcenter").gravity == M_GRAVITY_HCENTER);
    assert_truth(try_parse_layout("gravity-fill").gravity == M_GRAVITY_FILL);
    assert_truth(try_parse_layout("gravity-vfill").gravity == M_GRAVITY_VFILL);
    assert_truth(try_parse_layout("gravity-hfill").gravity == M_GRAVITY_HFILL);

    assert_truth(try_parse_layout("g-reset").gravity == M_GRAVITY_NONE);
    assert_truth(try_parse_layout("g-s").gravity == M_GRAVITY_START);
    assert_truth(try_parse_layout("g-start").gravity == M_GRAVITY_START);
    assert_truth(try_parse_layout("g-e").gravity == M_GRAVITY_END);
    assert_truth(try_parse_layout("g-end").gravity == M_GRAVITY_END);
    assert_truth(try_parse_layout("g-t").gravity == M_GRAVITY_TOP);
    assert_truth(try_parse_layout("g-top").gravity == M_GRAVITY_TOP);
    assert_truth(try_parse_layout("g-b").gravity == M_GRAVITY_BOTTOM);
    assert_truth(try_parse_layout("g-bottom").gravity == M_GRAVITY_BOTTOM);
    assert_truth(try_parse_layout("g-center").gravity == M_GRAVITY_CENTER);
    assert_truth(try_parse_layout("g-vcenter").gravity == M_GRAVITY_VCENTER);
    assert_truth(try_parse_layout("g-hcenter").gravity == M_GRAVITY_HCENTER);
    assert_truth(try_parse_layout("g-fill").gravity == M_GRAVITY_FILL);
    assert_truth(try_parse_layout("g-vfill").gravity == M_GRAVITY_VFILL);
    assert_truth(try_parse_layout("g-hfill").gravity == M_GRAVITY_HFILL);
}

TEST(ui_parse_layout_placement)
{
    assert_truth(try_parse_layout("placement-reset").placement == M_GRAVITY_NONE);
    assert_truth(try_parse_layout("placement-s").placement == M_GRAVITY_START);
    assert_truth(try_parse_layout("placement-start").placement == M_GRAVITY_START);
    assert_truth(try_parse_layout("placement-e").placement == M_GRAVITY_END);
    assert_truth(try_parse_layout("placement-end").placement == M_GRAVITY_END);
    assert_truth(try_parse_layout("placement-t").placement == M_GRAVITY_TOP);
    assert_truth(try_parse_layout("placement-top").placement == M_GRAVITY_TOP);
    assert_truth(try_parse_layout("placement-b").placement == M_GRAVITY_BOTTOM);
    assert_truth(try_parse_layout("placement-bottom").placement == M_GRAVITY_BOTTOM);
    assert_truth(try_parse_layout("placement-center").placement == M_GRAVITY_CENTER);
    assert_truth(try_parse_layout("placement-vcenter").placement == M_GRAVITY_VCENTER);
    assert_truth(try_parse_layout("placement-hcenter").placement == M_GRAVITY_HCENTER);
    assert_truth(try_parse_layout("placement-fill").placement == M_GRAVITY_FILL);
    assert_truth(try_parse_layout("placement-vfill").placement == M_GRAVITY_VFILL);
    assert_truth(try_parse_layout("placement-hfill").placement == M_GRAVITY_HFILL);

    assert_truth(try_parse_layout("place-reset").placement == M_GRAVITY_NONE);
    assert_truth(try_parse_layout("place-s").placement == M_GRAVITY_START);
    assert_truth(try_parse_layout("place-start").placement == M_GRAVITY_START);
    assert_truth(try_parse_layout("place-e").placement == M_GRAVITY_END);
    assert_truth(try_parse_layout("place-end").placement == M_GRAVITY_END);
    assert_truth(try_parse_layout("place-t").placement == M_GRAVITY_TOP);
    assert_truth(try_parse_layout("place-top").placement == M_GRAVITY_TOP);
    assert_truth(try_parse_layout("place-b").placement == M_GRAVITY_BOTTOM);
    assert_truth(try_parse_layout("place-bottom").placement == M_GRAVITY_BOTTOM);
    assert_truth(try_parse_layout("place-center").placement == M_GRAVITY_CENTER);
    assert_truth(try_parse_layout("place-vcenter").placement == M_GRAVITY_VCENTER);
    assert_truth(try_parse_layout("place-hcenter").placement == M_GRAVITY_HCENTER);
    assert_truth(try_parse_layout("place-fill").placement == M_GRAVITY_FILL);
    assert_truth(try_parse_layout("place-vfill").placement == M_GRAVITY_VFILL);
    assert_truth(try_parse_layout("place-hfill").placement == M_GRAVITY_HFILL);
}

TEST(ui_parse_layout_spacing)
{
    assert_truth(try_parse_layout("padding-3").padding.start > 2.9);
    assert_truth(try_parse_layout("padding-3").padding.end > 2.9);
    assert_truth(try_parse_layout("padding-3").padding.top > 2.9);
    assert_truth(try_parse_layout("padding-3").padding.bottom > 2.9);

    assert_truth(try_parse_layout("p-3").padding.start > 2.9);
    assert_truth(try_parse_layout("p-3").padding.end > 2.9);
    assert_truth(try_parse_layout("p-3").padding.top > 2.9);
    assert_truth(try_parse_layout("p-3").padding.bottom > 2.9);

    assert_truth(try_parse_layout("padding-start-3").padding.start > 2.9);
    assert_truth(try_parse_layout("padding-end-3").padding.end > 2.9);
    assert_truth(try_parse_layout("padding-top-3").padding.top > 2.9);
    assert_truth(try_parse_layout("padding-bottom-3").padding.bottom > 2.9);

    assert_truth(try_parse_layout("p-s-3").padding.start > 2.9);
    assert_truth(try_parse_layout("p-e-3").padding.end > 2.9);
    assert_truth(try_parse_layout("p-t-3").padding.top > 2.9);
    assert_truth(try_parse_layout("p-b-3").padding.bottom > 2.9);

    assert_truth(try_parse_layout("margin-3").margin.start > 2.9);
    assert_truth(try_parse_layout("margin-3").margin.end > 2.9);
    assert_truth(try_parse_layout("margin-3").margin.top > 2.9);
    assert_truth(try_parse_layout("margin-3").margin.bottom > 2.9);

    assert_truth(try_parse_layout("m-3").margin.start > 2.9);
    assert_truth(try_parse_layout("m-3").margin.end > 2.9);
    assert_truth(try_parse_layout("m-3").margin.top > 2.9);
    assert_truth(try_parse_layout("m-3").margin.bottom > 2.9);

    assert_truth(try_parse_layout("margin-start-3").margin.start > 2.9);
    assert_truth(try_parse_layout("margin-end-3").margin.end > 2.9);
    assert_truth(try_parse_layout("margin-top-3").margin.top > 2.9);
    assert_truth(try_parse_layout("margin-bottom-3").margin.bottom > 2.9);

    assert_truth(try_parse_layout("m-s-3").margin.start > 2.9);
    assert_truth(try_parse_layout("m-e-3").margin.end > 2.9);
    assert_truth(try_parse_layout("m-t-3").margin.top > 2.9);
    assert_truth(try_parse_layout("m-b-3").margin.bottom > 2.9);
}

TEST(ui_parse_layout_flow)
{
    assert_truth(try_parse_layout("ltr").flow == M_FLOW_LEFT_TO_RIGHT);
    assert_truth(try_parse_layout("rtl").flow == M_FLOW_RIGHT_TO_LEFT);
    assert_truth(try_parse_layout("ttb").flow == M_FLOW_TOP_TO_BOTTOM);
    assert_truth(try_parse_layout("btt").flow == M_FLOW_BOTTOM_TO_TOP);
}
