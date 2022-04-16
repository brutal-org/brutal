#include "dirty.h"

void gfx_dirty_init(GfxDirty *self, Alloc *alloc)
{
    vec_init(self, alloc);
}

void gfx_dirty_deinit(GfxDirty *self)
{
    vec_deinit(self);
}

void gfx_dirty_rect(GfxDirty *self, MRectf rect)
{
    vec_foreach(dirty, self)
    {
        if (m_rectf_collide_rect(*dirty, rect))
        {
            MRectf rects[4];
            m_rectf_substract(rect, *dirty, rects);

            for (int i = 0; i < 4; i++)
            {
                if (!m_rectf_empty(rects[i]))
                    gfx_dirty_rect(self, rects[i]);
            }

            return;
        }
    }

    vec_push(self, rect);
}

bool gfx_dirty_any(GfxDirty *self)
{
    return vec_len(self) > 0;
}

void gfx_dirty_clear(GfxDirty *self)
{
    vec_clear(self);
}
