#include "protocol.h"
#include <brutal/log.h>
#include <efi/utils.h>
#include "efi/bs.h"

void query_gop_modes(EFIGraphicsOutputProtocol *gop, u64 num_modes, u64 native_mode)
{

    EFIGraphicsOutputModeInfo *info;
    u64 size_of_info;

    for (u64 i = 0; i < num_modes; i++)
    {
        gop->query_mode(gop, i, &size_of_info, &info);
        efi_print("mode {} width {} height {} format {}{}", i, info->horizontal_resolution, info->vertical_resolution, info->pixel_format, i == native_mode ? "(current)" : "");
    }
}

HandoverFramebuffer get_framebuffer(EFIBootServices *bs)
{
    EFIGUID gop_guid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    EFIGraphicsOutputProtocol *gop;

    bs->locate_protocol(&gop_guid, NULL, (void **)&gop);

    EFIGraphicsOutputModeInfo *info;

    u64 size_of_info, num_modes, native_mode;

    auto status = gop->query_mode(gop, gop->mode == NULL ? 0 : gop->mode->mode, &size_of_info, &info);

    if (status == EFI_NOT_STARTED)
    {
        status = gop->set_mode(gop, 0);
    }

    if (status != EFI_SUCCESS)
    {
        panic("Couldn't get framebuffer native mode");
    }

    else
    {
        native_mode = gop->mode->mode;
        num_modes = gop->mode->max_mode;
    }

    auto addr = gop->mode->framebuffer_base;
    auto width = gop->mode->info->horizontal_resolution;
    auto height = gop->mode->info->vertical_resolution;
    auto pitch = gop->mode->info->pixels_per_scan_line * gop->mode->info->pixel_format;

    return (HandoverFramebuffer) {true, addr, width, height, pitch, gop->mode->info->pixel_format };
}

Handover get_handover(EFISystemTable *st)
{

    auto bs = st->boot_services;
    
    auto fb = get_framebuffer(bs);
    //efi_print("FB info: addr = {x}, width = {}, height = {}, pitch = {}, bpp = {}", (void*)fb.addr, fb.width, fb.height, fb.pitch, fb.bpp);
    
    return (Handover){.framebuffer = fb};
}
