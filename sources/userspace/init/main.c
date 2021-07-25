#include <brutal/base.h>
#include <brutal/codec/tga.h>
#include <brutal/gfx.h>
#include <brutal/log.h>
#include <handover/handover.h>
#include <syscalls/syscalls.h>

static void display_bootimage(Handover *handover)
{
    // Open the framebuffer

    auto fb = &handover->framebuffer;

    size_t fb_size = fb->height * fb->pitch;

    BrCreateArgs fb_obj = {
        .type = BR_OBJECT_MEMORY,
        .mem_obj = {
            .addr = fb->addr,
            .size = fb_size,
            .flags = BR_MOBJ_PMM,
        },
    };

    assert_truth(br_create(&fb_obj) == BR_SUCCESS);

    BrMapArgs fb_map = {
        .space = BR_SPACE_SELF,
        .mobj = fb_obj.handle,
        .flags = BR_MEM_WRITABLE,
    };

    assert_truth(br_map(&fb_map) == BR_SUCCESS);

    GfxSurface fb_surface = {
        .width = fb->width,
        .height = fb->height,
        .pitch = fb->pitch,
        .format = GFX_PIXEL_FORMAT_RGBA8888,
        .buffer = (void *)fb_map.vaddr,
        .size = fb_size,
    };

    // Open the bootimage

    auto img = handover_find_module(handover, str_cast("bootimg"));
    assert_not_null(img);

    BrCreateArgs img_obj = {
        .type = BR_OBJECT_MEMORY,
        .mem_obj = {
            .addr = img->addr,
            .size = img->size,
            .flags = BR_MOBJ_PMM,
        },
    };

    assert_truth(br_create(&img_obj) == BR_SUCCESS);

    BrMapArgs img_map = {
        .space = BR_SPACE_SELF,
        .mobj = img_obj.handle,
        .flags = BR_MEM_WRITABLE,
    };

    assert_truth(br_map(&img_map) == BR_SUCCESS);

    GfxSurface img_surface = tga_decode_in_memory((void *)img_map.vaddr, img_map.size);

    // Display the image

    gfx_surface_clear(fb_surface, GFX_COLOR_BLACK);

    gfx_surface_copy(
        fb_surface,
        img_surface,
        fb_surface.width / 2 - img_surface.width / 2,
        fb_surface.height / 2 - img_surface.height / 2);

    // Cleanup

    br_close(&(BrCloseArgs){.handle = img_obj.handle});
    br_close(&(BrCloseArgs){.handle = fb_obj.handle});

    br_unmap(&(BrUnmapArgs){.space = BR_SPACE_SELF, .vaddr = fb_map.vaddr, fb_map.size});
    br_unmap(&(BrUnmapArgs){.space = BR_SPACE_SELF, .vaddr = img_map.vaddr, img_map.size});
}

int br_entry(Handover *handover)
{
    log("Handover at {#p}", (void *)handover);

    handover_dump(handover);

    if (handover->framebuffer.present)
    {
        display_bootimage(handover);
    }

    while (1)
    {
    }

    return 0;
}
