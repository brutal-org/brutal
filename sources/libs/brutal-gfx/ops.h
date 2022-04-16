#pragma once

#include "buf.h"

void gfx_ops_copy(GfxBuf dst, GfxBuf src, int x, int y);

void gfx_ops_flip(GfxBuf dst, GfxBuf src, MRectf rect);

void gfx_ops_clear(GfxBuf self, GfxColor color);

void gfx_ops_rect(GfxBuf self, MRectf rect, GfxColor color);
