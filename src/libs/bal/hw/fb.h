#pragma once

#include <bal/hw/mem.h>

typedef enum
{
    BAL_FB_FMT_UNKNOWN,

    BAL_FB_FMT_GS8,
    BAL_FB_FMT_RGB888,
    BAL_FB_FMT_RGBA8888,
    BAL_FB_FMT_BGRA8888,
} BalFbFmt;

static inline size_t bal_fb_fmt_size(BalFbFmt fmt)
{
    switch (fmt)
    {
    case BAL_FB_FMT_GS8:
        return 1;
    case BAL_FB_FMT_RGB888:
        return 3;

    case BAL_FB_FMT_RGBA8888:
    case BAL_FB_FMT_BGRA8888:
        return 4;

    default:
        return 0;
    }
}

typedef struct
{
    int width;
    int height;
    int pitch;
    BalFbFmt fmt;
    BalMem mem;
} BalFb;

MaybeError bal_fb_init(BalFb *self, int width, int height, BalFbFmt fmt);

MaybeError bal_fb_init_mobj(BalFb *self, BrHandle handle, int width, int height, int pitch, BalFbFmt fmt);

MaybeError bal_fb_init_pmm(BalFb *self, uintptr_t addr, size_t size, int width, int height, int pitch, BalFbFmt fmt);

MaybeError bal_fb_init_mobj(BalFb *self, BrHandle handle, int width, int height, int pitch, BalFbFmt fmt);

MaybeError bal_fb_deinit(BalFb *self);

void bal_fb_dup(BalFb *dst, BalFb *src);

BrResult bal_fb_map(BalFb *self);

BrResult bal_fb_unmap(BalFb *self);

void bal_fb_pack(IpcPack *pack, const BalFb *self);

void bal_fb_unpack(IpcUnpack *pack, BalFb *self);
