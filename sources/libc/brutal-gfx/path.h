#pragma once

#include <brutal/math/Vec2.h>

typedef struct
{

} GfxPathBuffer;

typedef struct
{
    size_t len;
    Vec2f *points;
} GfxPath;

void gfx_path_move_to();

void gfx_path_line_to();

void gfx_path_cubic_bezier_to();

void gfx_path_quadratic_beziez_to();

void gfx_path_arc_to();
