#include <brutal/gfx/cache.h>

void gfx_cache_init(GfxCache *self, Alloc *alloc)
{
    linear_buffer_init(&self->commands, alloc);

    grid_init(&self->front_hashgrid, 16, 16, alloc);
    grid_init(&self->back_hashgrid, 16, 16, alloc);
}

void gfx_cache_deinit(GfxCache *self)
{
    grid_deinit(&self->back_hashgrid);
    grid_deinit(&self->front_hashgrid);

    linear_buffer_deinit(&self->commands);
}

void gfx_cache_begin(GfxCache *self, GfxSurface surface)
{
    grid_resize(
        &self->front_hashgrid,
        MIN(surface.width / GFX_CACHE_CELL_SIZE, 1),
        MIN(surface.height / GFX_CACHE_CELL_SIZE, 1));

    grid_resize(
        &self->back_hashgrid,
        MIN(surface.width / GFX_CACHE_CELL_SIZE, 1),
        MIN(surface.height / GFX_CACHE_CELL_SIZE, 1));
}

void gfx_cache_end(GfxCache *self, MAYBE_UNUSED GfxSurface surface)
{
    linear_buffer_foreach(command, &self->commands)
    {
        switch (command->type)
        {
        case GFX_COMMAND_RECT:
            todo$("Implement RECT command.");
            break;

        case GFX_COMMAND_LINE:
            todo$("Implement LINE command.");
            break;

        case GFX_COMMAND_TEXT:
            todo$("Implement TEXT command.");
            break;

        case GFX_COMMAND_POLY:
            todo$("Implement POLY command.");
            break;

        default:
            assert_unreachable();
            break;
        }
    }

    linear_buffer_clear(&self->commands);
}

void gfx_cache_invalidate(GfxCache *self)
{
    grid_fill(&self->front_hashgrid, 0x0);
    grid_fill(&self->back_hashgrid, 0x0);
}

void gfx_cache_rect(GfxCache *self, Rectf rect, GfxStroke stroke, GfxPaint fill, Trans2 transform)
{
    GfxRectCommand command = (GfxRectCommand){
        .base = (GfxCommand){
            .type = GFX_COMMAND_RECT,
            .bound = rect,
            .stroke = stroke,
            .fill = fill,
            .transform = transform,
        },
        .rect = rect,
    };

    linear_buffer_push(&self->commands, command);
}

// void gfx_cache_line(GfxCache *self, Edgef line, GfxStroke stroke, Trans2 transform)
// {
// }
//
// void gfx_cache_text(GfxCache *self, Str str, GfxPaint fill, Trans2 transform)
// {
// }
//
// void gfx_cache_poly(GfxCache *self, Edges edges, GfxStroke stroke, GfxPaint fill, Trans2 transform)
// {
// }
