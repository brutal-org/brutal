/**
 * Copyright (c) 2014 rxi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */

#include <brutal-mem>

#include "vec.h"

void vec_init_impl(VecImpl *impl, int data_size, Alloc *alloc)
{
    *impl = (VecImpl){
        .data_size = data_size,
        .alloc = alloc,
    };
}

void vec_deinit_impl(VecImpl *impl)
{
    if (impl->data)
    {
        alloc_release(impl->alloc, impl->data);
    }

    *impl = (VecImpl){};
}

bool vec_realloc_impl(VecImpl *impl, int new_len)
{
    void *ptr = alloc_acquire(impl->alloc, new_len * impl->data_size);

    if (ptr == nullptr)
    {
        return false;
    }

    if (impl->data != nullptr)
    {
        mem_cpy(ptr, impl->data, m_min(impl->len, new_len) * impl->data_size);
        alloc_release(impl->alloc, impl->data);
    }

    impl->data = (char *)ptr;
    impl->capacity = new_len;

    return true;
}

bool vec_expand_impl(VecImpl *impl)
{
    if (impl->len + 1 > impl->capacity)
    {
        int n = (impl->capacity == 0) ? 1 : impl->capacity * 2;
        return vec_realloc_impl(impl, n);
    }
    else
    {
        return true;
    }
}

bool vec_reserve_impl(VecImpl *impl, int n)
{
    if (n > impl->capacity)
    {
        return vec_realloc_impl(impl, n);
    }
    else
    {
        return true;
    }
}

bool vec_reserve_po2_impl(VecImpl *impl, int n)
{
    if (n == 0)
    {
        return true;
    }

    int n2 = 1;
    while (n2 < n)
    {
        n2 <<= 1;
    }

    return vec_reserve_impl(impl, n2);
}

bool vec_compact_impl(VecImpl *impl)
{
    if (impl->len == 0)
    {
        alloc_release(impl->alloc, impl->data);
        impl->data = nullptr;
        impl->capacity = 0;

        return true;
    }
    else
    {
        int n = impl->len;
        return vec_realloc_impl(impl, n);
    }
}

bool vec_insert_impl(VecImpl *impl, int idx)
{
    int err = vec_expand_impl(impl);

    if (err)
    {
        return err;
    }

    mem_move(impl->data + (idx + 1) * impl->data_size,
             impl->data + idx * impl->data_size,
             (impl->len - idx) * impl->data_size);

    return 0;
}

void vec_splice_impl(VecImpl *impl, int start, int count)
{
    mem_move(impl->data + start * impl->data_size, impl->data + (start + count) * impl->data_size,
             (impl->len - start - count) * impl->data_size);
}

void vec_swapsplice_impl(VecImpl *impl, int start, int count)
{
    mem_move(impl->data + start * impl->data_size,
             impl->data + (impl->len - count) * impl->data_size,
             count * impl->data_size);
}

void vec_swap_impl(VecImpl *impl, int idx1, int idx2)
{
    if (idx1 == idx2)
    {
        return;
    }

    unsigned char *a = (unsigned char *)impl->data + idx1 * impl->data_size;
    unsigned char *b = (unsigned char *)impl->data + idx2 * impl->data_size;
    int count = impl->data_size;

    while (count--)
    {
        unsigned char tmp = *a;
        *a = *b;
        *b = tmp;
        a++, b++;
    }
}
