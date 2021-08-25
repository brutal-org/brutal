#include <brutal/codec/tga.h>
#include <brutal/gfx.h>
#include <brutal/log.h>
#include <elf/exec.h>
#include <handover/handover.h>
#include <syscalls/helpers.h>
#include <syscalls/syscalls.h>

static void display_bootimage(Handover const *handover)
{
    // Open the framebuffer

    auto fb = &handover->framebuffer;

    size_t fb_size = fb->height * fb->pitch;

    BrCreateArgs fb_obj = {
        .type = BR_OBJECT_MEMORY,
        .mem_obj = {
            .addr = fb->addr,
            .size = fb_size,
            .flags = BR_MEM_OBJ_PMM,
        },
    };

    assert_truth(br_create(&fb_obj) == BR_SUCCESS);

    BrMapArgs fb_map = {
        .space = BR_SPACE_SELF,
        .mem_obj = fb_obj.handle,
        .flags = BR_MEM_WRITABLE,
    };

    assert_truth(br_map(&fb_map) == BR_SUCCESS);

    GfxSurface fb_surface = {
        .width = fb->width,
        .height = fb->height,
        .pitch = fb->pitch,
        .format = GFX_PIXEL_FORMAT_BGRA8888,
        .buffer = (void *)fb_map.vaddr,
        .size = fb_size,
    };

    // Open the bootimage

    auto img = handover_find_module(handover, str$("bootimg"));
    assert_not_null(img);

    BrCreateArgs img_obj = {
        .type = BR_OBJECT_MEMORY,
        .mem_obj = {
            .addr = img->addr,
            .size = img->size,
            .flags = BR_MEM_OBJ_PMM,
        },
    };

    assert_truth(br_create(&img_obj) == BR_SUCCESS);

    BrMapArgs img_map = {
        .space = BR_SPACE_SELF,
        .mem_obj = img_obj.handle,
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

    brh_close(img_obj.handle);
    brh_close(fb_obj.handle);

    assert_truth(br_unmap(&(BrUnmapArgs){
                     .space = BR_SPACE_SELF,
                     .vaddr = fb_map.vaddr,
                     .size = fb_map.size,
                 }) == BR_SUCCESS);

    assert_truth(br_unmap(&(BrUnmapArgs){
                     .space = BR_SPACE_SELF,
                     .vaddr = img_map.vaddr,
                     .size = img_map.size,
                 }) == BR_SUCCESS);
}

BrTask srv_run(Handover const *handover, Str name)
{
    auto elf = handover_find_module(handover, name);

    assert_not_null(elf);

    BrCreateArgs elf_obj = {
        .type = BR_OBJECT_MEMORY,
        .mem_obj = {
            .addr = elf->addr,
            .size = elf->size,
            .flags = BR_MEM_OBJ_PMM,
        },
    };

    assert_truth(br_create(&elf_obj) == BR_SUCCESS);

    auto task_handle = elf_exec(elf_obj.handle, name);

    brh_close(elf_obj.handle);

    log$("Service '{}' created!", name);

    return task_handle;
}

int br_entry(Handover const *handover)
{
    log$("Handover at {#p}", (void *)handover);

    handover_dump(handover);

    if (handover->framebuffer.present)
    {
        display_bootimage(handover);
    }

    srv_run(handover, str$("posix"));
    BrTask echo_srv = srv_run(handover, str$("echo"));

    while (true)
    {
        br_ipc(&(BrIpcArgs){
            .to = echo_srv,
            .flags = BR_IPC_SEND | BR_IPC_RECV | BR_IPC_BLOCK,
            .deadline = BR_DEADLINE_INFINITY,
            .msg = {
                .arg = {
                    0x1,
                    0x2,
                    0x3,
                    0x4,
                    0x5,
                },
            },
        });

        log$("Got message");
    }

    return 0;
}
