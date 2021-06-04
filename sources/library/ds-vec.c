/**
 * Copyright (c) 2014 rxi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */

#include <base-std.h>
#include <ds-vec.h>
#include <mem.h>

bool vec_details_realloc(struct vec_details *details, int new_length)
{
    void *ptr = alloc_acquire(details->alloc, new_length * details->data_size);

    if (ptr == nullptr)
    {
        return false;
    }

    alloc_commit(details->alloc, ptr);

    if (*details->data != nullptr)
    {
        mem_cpy(ptr, *details->data, MIN((*details->length), new_length) * details->data_size);
        alloc_release(details->alloc, *details->data);
    }

    *details->data = ptr;
    *details->capacity = new_length;

    return true;
}

bool vec_details_expand(struct vec_details *details)
{
    if (*details->length + 1 > *details->capacity)
    {
        int n = (*details->capacity == 0) ? 1 : *details->capacity << 1;
        return vec_details_realloc(details, n);
    }
    else
    {
        return true;
    }
}

bool vec_details_reserve(struct vec_details *details, int n)
{
    if (n > *details->capacity)
    {
        return vec_details_realloc(details, n);
    }
    else
    {
        return true;
    }
}

bool vec_details_reserve_po2(struct vec_details *details, int n)
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

    return vec_details_reserve(details, n2);
}

bool vec_details_compact(struct vec_details *details)
{
    if (*details->length == 0)
    {
        alloc_release(details->alloc, details->data);
        *details->data = NULL;
        *details->capacity = 0;

        return true;
    }
    else
    {
        int n = *details->length;
        return vec_details_realloc(details, n);
    }
}

bool vec_details_insert(struct vec_details *details, int idx)
{
    int err = vec_details_expand(details);

    if (err)
    {
        return err;
    }

    mem_move(*details->data + (idx + 1) * details->data_size,
             *details->data + idx * details->data_size,
             (*details->length - idx) * details->data_size);

    return 0;
}

void vec_details_splice(struct vec_details *details, int start, int count)
{
    mem_move(*details->data + start * details->data_size, *details->data + (start + count) * details->data_size,
             (*details->length - start - count) * details->data_size);
}

void vec_details_swapsplice(struct vec_details *details, int start, int count)
{
    mem_move(*details->data + start * details->data_size,
             *details->data + (*details->length - count) * details->data_size,
             count * details->data_size);
}

void vec_details_swap(struct vec_details *details, int idx1, int idx2)
{
    if (idx1 == idx2)
    {
        return;
    }

    unsigned char *a = (unsigned char *)*details->data + idx1 * details->data_size;
    unsigned char *b = (unsigned char *)*details->data + idx2 * details->data_size;
    int count = details->data_size;

    while (count--)
    {
        unsigned char tmp = *a;
        *a = *b;
        *b = tmp;
        a++, b++;
    }
}