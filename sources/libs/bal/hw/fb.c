#include <bal/hw/fb.h>

MaybeError bal_fb_init(BalFb *self, int width, int height, GfxFmt fmt)
{
    *self = (BalFb){
        .width = width,
        .height = height,
        .pitch = gfx_fmt_size(fmt) * width,
        .fmt = fmt,
    };

    return bal_mem_init(&self->mem, self->pitch * height);
}

MaybeError bal_fb_init_mobj(BalFb *self, BrHandle handle, int width, int height, int pitch, GfxFmt fmt)
{
    *self = (BalFb){
        .width = width,
        .height = height,
        .pitch = pitch,
        .fmt = fmt,
    };

    return bal_mem_init_mobj(&self->mem, handle);
}

MaybeError bal_fb_init_pmm(BalFb *self, uintptr_t addr, size_t size, int width, int height, int pitch, GfxFmt fmt)
{
    *self = (BalFb){
        .width = width,
        .height = height,
        .pitch = pitch,
        .fmt = fmt,
    };

    return bal_mem_init_pmm(&self->mem, addr, size);
}

MaybeError bal_fb_deinit(BalFb *self)
{
    return bal_mem_deinit(&self->mem);
}

void bal_fb_dup(BalFb *dst, BalFb *src)
{
    *dst = (BalFb){
        .width = src->width,
        .height = src->height,
        .pitch = src->pitch,
        .fmt = src->fmt,
    };

    bal_mem_dup(&dst->mem, &src->mem);
}

BrResult bal_fb_map(BalFb *self)
{
    return bal_mem_map(&self->mem);
}

BrResult bal_fb_unmap(BalFb *self)
{
    return bal_mem_unmap(&self->mem);
}

GfxBuf bal_fb_buf(BalFb *self)
{
    return (GfxBuf){
        .width = self->width,
        .height = self->height,
        .pitch = self->pitch,
        .fmt = self->fmt,
        .buf = self->mem.buf,
    };
}

void bal_fb_pack(IpcPack *pack, const BalFb *self)
{
    ipc_pack_pod(pack, &self->width);
    ipc_pack_pod(pack, &self->height);
    ipc_pack_pod(pack, &self->pitch);
    ipc_pack_pod(pack, &self->fmt);
    bal_mem_pack(pack, &self->mem);
}

void bal_fb_unpack(IpcUnpack *pack, BalFb *self)
{
    ipc_unpack_pod(pack, &self->width);
    ipc_unpack_pod(pack, &self->height);
    ipc_unpack_pod(pack, &self->pitch);
    ipc_unpack_pod(pack, &self->fmt);
    bal_mem_unpack(pack, &self->mem);
}
