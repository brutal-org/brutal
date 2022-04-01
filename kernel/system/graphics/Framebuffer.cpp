#include <abi/Paths.h>

#include "system/Streams.h"
#include <libmath/MinMax.h>

#include "archs/Arch.h"

#include "system/graphics/Graphics.h"
#include "system/interrupts/Interupts.h"
#include "system/node/Node.h"
#include "system/scheduling/Scheduler.h"

static uintptr_t _framebuffer_physical = 0;
static uintptr_t _framebuffer_virtual = 0;
static int _framebuffer_width = 0;
static int _framebuffer_height = 0;
static int _framebuffer_pitch = 0;
static int _framebuffer_bpp = 0;

struct Framebuffer : public FsNode
{
private:
    /* data */
public:
    Framebuffer(/* args */) : FsNode(HJ_FILE_TYPE_DEVICE)
    {
    }

    ~Framebuffer()
    {
    }

    HjResult call(FsHandle &handle, IOCall iocall, void *args) override
    {
        UNUSED(handle);

        if (iocall == IOCALL_DISPLAY_GET_MODE)
        {
            IOCallDisplayModeArgs *mode = (IOCallDisplayModeArgs *)args;

            mode->width = _framebuffer_width;
            mode->height = _framebuffer_height;

            return SUCCESS;
        }
        else if (iocall == IOCALL_DISPLAY_BLIT)
        {
            IOCallDisplayBlitArgs *blit = (IOCallDisplayBlitArgs *)args;

            InterruptsRetainer retainer;

            for (int y = MAX(0, blit->blit_y); y < MIN(_framebuffer_height, blit->blit_y + blit->blit_height); y++)
            {
                for (int x = MAX(0, blit->blit_x); x < MIN(_framebuffer_width, blit->blit_x + blit->blit_width); x++)
                {
                    uint32_t pixel = blit->buffer[y * blit->buffer_width + x];

                    uint32_t converted_pixel = ((pixel >> 16) & 0x000000ff) |
                                               ((pixel)&0xff00ff00) |
                                               ((pixel << 16) & 0x00ff0000);

                    ((uint32_t *)_framebuffer_virtual)[y * (_framebuffer_pitch / 4) + x] = converted_pixel;
                }
            }

            return SUCCESS;
        }
        else
        {
            return ERR_INAPPROPRIATE_CALL_FOR_DEVICE;
        }
    }
};

void framebuffer_initialize(Handover *handover)
{
    InterruptsRetainer retainer;

    _framebuffer_width = handover->framebuffer_width;
    _framebuffer_height = handover->framebuffer_height;
    _framebuffer_pitch = handover->framebuffer_pitch;
    _framebuffer_bpp = handover->framebuffer_bpp;

    Kernel::logln("Framebuffer {}x{}", handover->framebuffer_width, handover->framebuffer_height);

    if (_framebuffer_width == 0 || _framebuffer_height == 0)
    {
        Kernel::logln("No framebuffer!");
        return;
    }

    _framebuffer_physical = handover->framebuffer_addr;
    _framebuffer_virtual = Arch::virtual_alloc(
                               Arch::kernel_address_space(),
                               (MemoryRange){
                                   _framebuffer_physical,
                                   PAGE_ALIGN_UP(_framebuffer_width * _framebuffer_height * sizeof(uint32_t)),
                               },
                               MEMORY_NONE)
                               .base();

    if (_framebuffer_virtual == 0)
    {
        Kernel::logln("Failed to allocate memory for the framebuffer!");
        return;
    }

    graphic_did_find_framebuffer(_framebuffer_virtual, _framebuffer_width, _framebuffer_height, _framebuffer_pitch, _framebuffer_bpp);

    scheduler_running()->domain().link(IO::Path::parse(FRAMEBUFFER_DEVICE_PATH), make<Framebuffer>());
}
