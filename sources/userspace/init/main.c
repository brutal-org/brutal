#include <brutal/base.h>
#include <brutal/codec/tga.h>
#include <brutal/gfx.h>
#include <brutal/log.h>
#include <elf/loader.h>
#include <handover/handover.h>
#include <syscalls/helpers.h>
#include <syscalls/syscalls.h>

static void display_bootimage(Handover *handover)
{
    // Open the framebuffer

    auto fb = &handover->framebuffer;

    size_t fb_size = fb->height * fb->pitch;

    log("Framebuffer memory size is {}kio", fb_size / 1024);

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

    auto img = handover_find_module(handover, str_cast("bootimg"));
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

    br_close(&(BrCloseArgs){.handle = img_obj.handle});
    br_close(&(BrCloseArgs){.handle = fb_obj.handle});

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

BrTask srv_run(Handover *handover, Str name)
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

    BrMapArgs elf_map = {
        .space = BR_SPACE_SELF,
        .mem_obj = elf_obj.handle,
        .flags = BR_MEM_WRITABLE,
    };

    assert_truth(br_map(&elf_map) == BR_SUCCESS);

    BrCreateArgs elf_space = {
        .type = BR_OBJECT_SPACE,
        .space = {
            .flags = BR_SPACE_NONE,
        },
    };

    assert_truth(br_create(&elf_space) == BR_SUCCESS);

    BrCreateArgs elf_task = {
        .type = BR_OBJECT_TASK,
        .task = {
            .name = str_cast_fix(StrFix128, name),
            .space = elf_space.handle,
            .caps = BR_CAP_ALL,
        },
    };

    assert_truth(br_create(&elf_task) == BR_SUCCESS);

    Elf64Header *elf_header = (Elf64Header *)elf_map.vaddr;

    elf_load(elf_space.handle, elf_header, elf_obj.handle);

    BrCreateArgs stack_obj = {
        .type = BR_OBJECT_MEMORY,
        .mem_obj = {
            .size = 0x4000,
            .flags = BR_MEM_OBJ_NONE,
        },
    };

    assert_truth(br_create(&stack_obj) == BR_SUCCESS);

    BrMapArgs stack_map = {
        .space = elf_space.handle,
        .mem_obj = stack_obj.handle,
        .vaddr = 0xC0000000 - 0x4000,
        .flags = BR_MEM_WRITABLE,
    };

    assert_truth(br_map(&stack_map) == BR_SUCCESS);

    assert_truth(br_start(&(BrStartArgs){
                     .task = elf_task.handle,
                     .ip = elf_header->entry,
                     .sp = 0xC0000000,
                 }) == BR_SUCCESS);

    assert_truth(br_unmap(&(BrUnmapArgs){
        .space = BR_SPACE_SELF,
        .vaddr = elf_map.vaddr,
        .size = elf_map.size,
    }));

    log("Service '{}' created!", name);

    return elf_task.handle;
}

int br_entry(Handover *handover)
{
    log("Handover at {#p}", (void *)handover);

    handover_dump(handover);

    if (handover->framebuffer.present)
    {
        display_bootimage(handover);
    }

    auto posix_task = srv_run(handover, str_cast("posix"));

    br_ipc(&(BrIpcArgs){
        .task = posix_task,
        .message = {
            .header = {
                .protocol = 0xC001C0DE,
                .type = 0xCAFEBABE,
                .size = 1,
            },
            .data = {0x69},
        },
        .timeout = 0,
        .flags = BR_IPC_SEND,
    });

    log("IPC messaged sent!");

    return 0;
}
