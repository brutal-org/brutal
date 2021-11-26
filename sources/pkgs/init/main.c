#include <bal/abi.h>
#include <bal/hw.h>
#include <bal/task.h>
#include <brutal/alloc.h>
#include <brutal/codec/tga.h>
#include <brutal/debug.h>
#include <brutal/gfx.h>
#include <protos/test/hello.h>

static void display_bootimage(Handover const *handover)
{
    // Open the framebuffer

    HandoverFramebuffer const *fb = &handover->framebuffer;

    size_t fb_size = fb->height * fb->pitch;

    BalMem fb_mem;
    bal_mem_init_pmm(&fb_mem, fb->addr, fb_size);

    GfxSurface fb_surface = {
        .width = fb->width,
        .height = fb->height,
        .pitch = fb->pitch,
        .format = GFX_PIXEL_FORMAT_BGRA8888,
        .buf = (void *)fb_mem.buf,
        .size = fb_size,
    };

    // Open the bootimage

    HandoverModule const *img = handover_find_module(handover, str$("bootimg"));
    assert_not_null(img);

    BalMem img_mem;
    bal_mem_init_pmm(&img_mem, img->addr, img->size);

    GfxSurface img_surface = tga_decode_in_memory((void *)img_mem.buf, img_mem.len);

    // Display the image

    gfx_surface_clear(fb_surface, GFX_COLOR_BLACK);

    gfx_surface_copy(
        fb_surface,
        img_surface,
        fb_surface.width / 2 - img_surface.width / 2,
        fb_surface.height / 2 - img_surface.height / 2);

    // Cleanup

    bal_mem_deinit(&img_mem);
    bal_mem_deinit(&fb_mem);
}

BrResult srv_run(Handover const *handover, Str name, BrExecArgs const *args, BrTaskInfos *infos)
{
    HandoverModule const *elf = handover_find_module(handover, name);
    assert_not_null(elf);

    BalMem elf_mem;
    bal_mem_init_pmm(&elf_mem, elf->addr, elf->size);
    BrResult result = bal_exec(&elf_mem, name, args, infos);
    bal_mem_deinit(&elf_mem);

    log$("Service '{}' created!", name);

    return result;
}

int br_entry_handover(Handover *handover)
{
    if (handover->framebuffer.present)
    {
        display_bootimage(handover);
    }

    BrTaskInfos posix_server = {};
    srv_run(
        handover,
        str$("posix"),
        &(BrExecArgs){
            .type = BR_START_CMAIN,
            .cmain = {
                .argc = 3,
                .argv = (Str[]){
                    str$("Hello"),
                    str$("friends"),
                    str$(":^)"),
                },
            },
        },
        &posix_server);

    BrTaskInfos acpi_server = {};
    srv_run(
        handover,
        str$("acpi"),
        &(BrExecArgs){
            .type = BR_START_HANDOVER,
            .handover = handover,
        },
        &acpi_server);

    BrTaskInfos pci_server = {};
    srv_run(
        handover,
        str$("pci"),
        &(BrExecArgs){
            .type = BR_START_HANDOVER,
            .handover = handover,
        },
        &pci_server);

    BrTaskInfos ps2_server = {};
    srv_run(
        handover,
        str$("ps2"),
        &(BrExecArgs){
            .type = BR_START_ARGS,
        },
        &ps2_server);

    BrTaskInfos hello_server = {};
    srv_run(
        handover,
        str$("hello"),
        &(BrExecArgs){
            .type = BR_START_CMAIN,
            .cmain = {
                .argc = 5,
                .argv = (Str[]){
                    str$("Hello"),
                    str$("world"),
                    str$("my"),
                    str$("friend"),
                    str$(":^)"),
                },
            },
        },
        &hello_server);

    IpcEv ev = {};
    br_ev_init(&ev, alloc_global());

    Str req = str$("Hello!");
    Str resp = nullstr;
    HelloError result = hello_hello(&ev, hello_server.tid, &req, &resp, alloc_global());
    log$("result is {}", result);
    log$("message is {}", resp);

    return br_ev_run(&ev);
}
