#pragma once

#include <brutal-gfx>

#include "hook-alloc.h"

/* --- Gfx Surface Hook ----------------------------------------------------- */

#define TEST_GFX_SURFACE_HOOK (0x932ba9ac9a4d7219)

static void *__test_use_gfx_surface_ctor(void *)
{
    GfxSurface *surface = alloc_make(test_use_alloc(), GfxSurface);
    gfx_surface_init(surface, 512, 512, GFX_FMT_RGBA8888, test_use_alloc());

    test_hook_ref(test_self(), TEST_ALLOC_HOOK);

    return surface;
}

static void __test_use_gfx_surface_dtor(void *surface)
{
    gfx_surface_deinit(surface);
    alloc_free(test_use_alloc(), surface);

    test_hook_deref(test_self(), TEST_ALLOC_HOOK);
}

static inline GfxSurface *test_use_gfx_surface(void)
{
    return test_hook_use(
        test_self(),
        TEST_GFX_SURFACE_HOOK,
        nullptr,
        __test_use_gfx_surface_ctor,
        __test_use_gfx_surface_dtor);
}

/* --- Gfx Hook ------------------------------------------------------------- */

#define TEST_GFX_HOOK (0x17c2235ee0c5c0a7)

static void *__test_use_gfx_ctor(void *)
{
    Gfx *gfx = alloc_make(test_use_alloc(), Gfx);
    gfx_init(gfx, test_use_alloc());
    gfx_begin(gfx, gfx_surface_buf(test_use_gfx_surface()));
    gfx_clear(gfx, GFX_BLACK);

    test_hook_ref(test_self(), TEST_ALLOC_HOOK);
    test_hook_ref(test_self(), TEST_GFX_SURFACE_HOOK);

    return gfx;
}

static void __test_use_gfx_dtor(void *gfx)
{
    gfx_end(gfx);
    gfx_deinit(gfx);
    alloc_free(test_use_alloc(), gfx);

    test_hook_deref(test_self(), TEST_GFX_SURFACE_HOOK);
    test_hook_deref(test_self(), TEST_ALLOC_HOOK);
}

MAYBE_UNUSED static inline Gfx *test_use_gfx(void)
{
    return test_hook_use(
        test_self(),
        TEST_GFX_HOOK,
        nullptr,
        __test_use_gfx_ctor,
        __test_use_gfx_dtor);
}
