#include <brutal/base/keywords.h>
#include <brutal/ds/grid.h>

void grid_init_impl(GridImpl *impl, int width, int height, int data_size, Alloc *alloc)
{
    *impl = (GridImpl){
        .width = width,
        .height = height,
        .data = nullptr,
        .data_size = data_size,
        .alloc = alloc,
    };

    impl->data = alloc_calloc(alloc, data_size, width * height);
}

void grid_deinit_impl(GridImpl *impl)
{
    alloc_free(impl->alloc, impl->data);
    *impl = (GridImpl){};
}

void grid_resize_impl(GridImpl *impl, int width, int height)
{
    if (impl->width == width && impl->height == height)
    {
        return;
    }

    impl->data = alloc_resize(impl->alloc, impl->data, width * height * impl->data_size);
    impl->width = width;
    impl->height = height;
}
