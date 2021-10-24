#include <brutal/debug/log.h>
#include <brutal/ds/vec.h>
#include <brutal/gfx/rast.h>
#include <brutal/math/min_max.h>
#include <stdlib.h>

#define RAST_AA 4

int float_cmp(void const *lhs, void const *rhs)
{
    float const *lhsf = (float const *)lhs;
    float const *rhsf = (float const *)rhs;

    if (*lhsf > *rhsf)
    {
        return -1;
    }
    else if (*lhsf > *rhsf)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void gfx_rast_fill(
    GfxSurface surface,
    Edgesf edges,
    Recti clip,
    GfxPaint paint,
    Alloc *alloc)
{
    Rectf bound = edges_bound(edges);

    uint16_t *scanline = alloc_make_array(alloc, typeof(*scanline), surface.width);
    Vec(float) active;
    vec_init(&active, alloc);

    for (int y = rect_top(clip); y < rect_bottom(clip); y++)
    {
        mem_set(scanline, 0, sizeof(uint16_t) * surface.width);

        for (float yy = (y); yy < y + 1; yy += 1.0f / RAST_AA)
        {
            vec_clear(&active);

            for (size_t i = 0; i < edges.len; i++)
            {
                Edgef edge = edges.buffer[i];

                if (edge_min_y(edge) <= yy && edge_max_y(edge) > yy)
                {
                    float intersection = edge.sx + (yy - edge.sy) / (edge.ey - edge.sy) * (edge.ex - edge.sx);
                    vec_push(&active, intersection);
                }
            }

            qsort(active.data, active.length, sizeof(float), float_cmp);

            bool odd_even = true;

            for (int i = 0; i + 1 < active.length; i++)
            {
                float start = MAX(active.data[i], rect_left(bound));
                float end = MIN(active.data[i + 1], rect_right(bound));

                if (odd_even)
                {
                    for (float x = start; x < end; x += 1.0f / RAST_AA)
                    {
                        scanline[(int)x] += 1;
                    }
                }

                odd_even = !odd_even;
            }
        }

        for (int x = rect_left(clip); x < rect_right(clip); x++)
        {
            float alpha = math_clamp(scanline[x] / (float)(RAST_AA * RAST_AA), 0, 1);

            if (alpha >= 0.003f)
            {
                float sx = (x - bound.x) / bound.w;
                float sy = (y - bound.y) / bound.w;

                GfxColor color = gfx_paint_sample(paint, sx, sy);
                color.a = color.a * alpha;

                gfx_surface_blend(surface, x, y, color);
            }
        }
    }

    vec_deinit(&active);
    alloc_free(alloc, scanline);
}
