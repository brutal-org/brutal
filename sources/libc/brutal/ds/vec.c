/**
 * Copyright (c) 2014 rxi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */

#include <brutal/base/std.h>
#include <brutal/ds/vec.h>
#include <brutal/mem.h>

bool vec_realloc_impl(struct vec_impl *impl, int new_length)
{
    void *ptr = alloc_acquire(impl->alloc, new_length * impl->data_size);

    if (ptr == nullptr)
    {
        return false;
    }

    alloc_commit(impl->alloc, ptr);

    if (*impl->data != nullptr)
    {
        mem_cpy(ptr, *impl->data, MIN((*impl->length), new_length) * impl->data_size);
        alloc_release(impl->alloc, *impl->data);
    }

    *impl->data = ptr;
    *impl->capacity = new_length;

    return true;
}

bool vec_expand_impl(struct vec_impl *impl)
{
    if (*impl->length + 1 > *impl->capacity)
    {
        int n = (*impl->capacity == 0) ? 1 : *impl->capacity << 1;
        return vec_realloc_impl(impl, n);
    }
    else
    {
        return true;
    }
}

bool vec_reserve_impl(struct vec_impl *impl, int n)
{
    if (n > *impl->capacity)
    {
        return vec_realloc_impl(impl, n);
    }
    else
    {
        return true;
    }
}

bool vec_reserve_po2_impl(struct vec_impl *impl, int n)
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

bool vec_compact_impl(struct vec_impl *impl)
{
    if (*impl->length == 0)
    {
        alloc_release(impl->alloc, impl->data);
        *impl->data = NULL;
        *impl->capacity = 0;

        return true;
    }
    else
    {
        int n = *impl->length;
        return vec_realloc_impl(impl, n);
    }
}

bool vec_insert_impl(struct vec_impl *impl, int idx)
{
    int err = vec_expand_impl(impl);

    if (err)
    {
        return err;
    }

    mem_move(*impl->data + (idx + 1) * impl->data_size,
             *impl->data + idx * impl->data_size,
             (*impl->length - idx) * impl->data_size);

    return 0;
}

void vec_splice_impl(struct vec_impl *impl, int start, int count)
{
    mem_move(*impl->data + start * impl->data_size, *impl->data + (start + count) * impl->data_size,
             (*impl->length - start - count) * impl->data_size);
}

void vec_swapsplice_impl(struct vec_impl *impl, int start, int count)
{
    mem_move(*impl->data + start * impl->data_size,
             *impl->data + (*impl->length - count) * impl->data_size,
             count * impl->data_size);
}

void vec_swap_impl(struct vec_impl *impl, int idx1, int idx2)
{
    if (idx1 == idx2)
    {
        return;
    }

    unsigned char *a = (unsigned char *)*impl->data + idx1 * impl->data_size;
    unsigned char *b = (unsigned char *)*impl->data + idx2 * impl->data_size;
    int count = impl->data_size;

    while (count--)
    {
        unsigned char tmp = *a;
        *a = *b;
        *b = tmp;
        a++, b++;
    }
}
