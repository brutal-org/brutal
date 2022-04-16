#pragma once

#include <bal/hw/mem.h>
#include <brutal-gfx/buf.h>

typedef struct
{
    int width;
    int height;
    int pitch;
    GfxFmt fmt;
    BalMem mem;
} BalFb;

MaybeError bal_fb_init(BalFb *self, int width, int height, GfxFmt fmt);

MaybeError bal_fb_init_mobj(BalFb *self, BrHandle handle, int width, int height, int pitch, GfxFmt fmt);

MaybeError bal_fb_init_pmm(BalFb *self, uintptr_t addr, size_t size, int width, int height, int pitch, GfxFmt fmt);

MaybeError bal_fb_init_mobj(BalFb *self, BrHandle handle, int width, int height, int pitch, GfxFmt fmt);

MaybeError bal_fb_deinit(BalFb *self);

void bal_fb_dup(BalFb *dst, BalFb *src);

BrResult bal_fb_map(BalFb *self);

BrResult bal_fb_unmap(BalFb *self);

GfxBuf bal_fb_buf(BalFb *self);

void bal_fb_pack(IpcPack *pack, const BalFb *self);

void bal_fb_unpack(IpcUnpack *pack, BalFb *self);
