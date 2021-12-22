#include <bal/hw.h>
#include <brutal/codec/tga.h>
#include "init/boot.h"

void boot_splashscreen(Handover const *handover)
{
    if (!handover->framebuffer.present)
    {
        return;
    }

    // Open the framebuffer

    HandoverFramebuffer const *fb = &handover->framebuffer;

    size_t fb_size = fb->height * fb->pitch;

    BalMem fb_mem;
    bal_mem_init_pmm(&fb_mem, fb->addr, fb_size);

    GfxBuf fb_surface = {
        .width = fb->width,
        .height = fb->height,
        .pitch = fb->pitch,
        .fmt = GFX_FMT_BGRA8888,
        .buf = (void *)fb_mem.buf,
        .size = fb_size,
    };

    // Open the bootimage

    HandoverModule const *img = handover_find_module(handover, str$("bootimg"));
    assert_not_null(img);

    BalMem img_mem;
    bal_mem_init_pmm(&img_mem, img->addr, img->size);

    GfxBuf img_surface = tga_decode_in_memory((void *)img_mem.buf, img_mem.len);

    // Display the image

    gfx_buf_clear(fb_surface, GFX_BLACK);

    gfx_buf_copy(
        fb_surface,
        img_surface,
        fb_surface.width / 2 - img_surface.width / 2,
        fb_surface.height / 2 - img_surface.height / 2);

    // Cleanup

    bal_mem_deinit(&img_mem);
    bal_mem_deinit(&fb_mem);
}
