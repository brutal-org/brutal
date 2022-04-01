
#include <abi/IOCall.h>
#include <abi/Paths.h>

#include <abi/Result.h>
#include <libgraphic/Framebuffer.h>
#include <libsystem/core/Plugs.h>

namespace Graphic
{

ResultOr<Box<Framebuffer>> Framebuffer::open()
{
    Handle handle;
    TRY(__plug_handle_open(&handle, FRAMEBUFFER_DEVICE_PATH, HJ_OPEN_READ | HJ_OPEN_WRITE));

    IOCallDisplayModeArgs mode_info = {};
    TRY(__plug_handle_call(&handle, IOCALL_DISPLAY_GET_MODE, &mode_info));

    auto bitmap_or_error = Bitmap::create_shared(mode_info.width, mode_info.height);

    if (!bitmap_or_error.success())
    {
        return bitmap_or_error.result();
    }

    return own<Framebuffer>(handle, bitmap_or_error.unwrap());
}

Framebuffer::Framebuffer(Handle handle, Ref<Bitmap> bitmap)
    : _handle(handle),
      _bitmap(bitmap)
{
}

Framebuffer::~Framebuffer()
{
    __plug_handle_close(&_handle);
}

HjResult Framebuffer::set_resolution(Math::Vec2i size)
{
    auto bitmap = TRY(Bitmap::create_shared(size.x(), size.y()));

    IOCallDisplayModeArgs mode_info = (IOCallDisplayModeArgs){size.x(), size.y()};

    TRY(__plug_handle_call(&_handle, IOCALL_DISPLAY_SET_MODE, &mode_info));

    _bitmap = bitmap;

    return SUCCESS;
}

void Framebuffer::mark_dirty(Math::Recti new_bound)
{
    new_bound = _bitmap->bound().clipped_with(new_bound);

    if (new_bound.is_empty())
    {
        return;
    }

    bool merged = false;

    _dirty_bounds.foreach([&](Math::Recti &region)
        {
            int region_area = region.area();
            int merge_area = region.merged_with(new_bound).area();

            if (region.colide_with(new_bound) && (region_area + new_bound.area() > merge_area))
            {
                region = region.merged_with(new_bound);
                merged = true;

                return Iter::STOP;
            }

            return Iter::CONTINUE;
        });

    if (!merged)
    {
        _dirty_bounds.push_back(new_bound);
    }
}

void Framebuffer::mark_dirty_all()
{
    _dirty_bounds.clear();
    mark_dirty(_bitmap->bound());
}

void Framebuffer::blit()
{
    if (_dirty_bounds.empty())
    {
        return;
    }

    _dirty_bounds.foreach([&](auto &bound)
        {
            IOCallDisplayBlitArgs args;

            args.buffer = reinterpret_cast<uint32_t *>(_bitmap->pixels());
            args.buffer_width = _bitmap->width();
            args.buffer_height = _bitmap->height();

            args.blit_x = bound.x();
            args.blit_y = bound.y();
            args.blit_width = bound.width();
            args.blit_height = bound.height();

            __plug_handle_call(&_handle, IOCALL_DISPLAY_BLIT, &args);

            return Iter::CONTINUE;
        });

    _dirty_bounds.clear();
}

} // namespace Graphic
