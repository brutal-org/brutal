#include <bruwutal/alloc/global.h>
#include <bruwutal/ui/frame.h>

UiView *ui_frame_create(void)
{
    return ui_view_create$(UiFrame);
}
