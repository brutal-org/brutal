#include <brutal/alloc/global.h>
#include <brutal/ui/frame.h>

UiView *ui_frame_create(void)
{
    return ui_view_create$(UiFrame);
}
