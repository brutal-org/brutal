#include <brutal-math>

#include "color.h"

static MRand rand;

GfxColor gfx_color_rand(uint8_t alpha)
{
    return gfx_rgba(
        m_rand_next_u8(&rand),
        m_rand_next_u8(&rand),
        m_rand_next_u8(&rand),
        alpha);
}
