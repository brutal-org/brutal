#pragma once

#include <brutal-alloc>
#include <brutal-mem>

typedef struct
{
    char *data;
    int len;

    Alloc *alloc;
    int data_size;
    int capacity;
} VecImpl;

#define Vec(T)         \
    union              \
    {                  \
        struct         \
        {              \
            T *data;   \
            int len;   \
        };             \
        VecImpl _impl; \
    }

void vec_init_impl(VecImpl *impl, int data_size, Alloc *alloc);

void vec_deinit_impl(VecImpl *impl);

bool vec_expand_impl(VecImpl *impl);

bool vec_reserve_impl(VecImpl *impl, int n);

bool vec_reserve_po2_impl(VecImpl *impl, int n);

bool vec_compact_impl(VecImpl *impl);

bool vec_insert_impl(VecImpl *impl, int idx);

void vec_splice_impl(VecImpl *impl, int start, int count);

void vec_swapsplice_impl(VecImpl *impl, int start, int count);

void vec_swap_impl(VecImpl *impl, int idx1, int idx2);

#define vec_init(v, alloc_) \
    vec_init_impl(impl$(v), sizeof(*(v)->data), alloc_)

#define vec_deinit(v) \
    vec_deinit_impl(impl$(v))

#define vec_push(v, val) \
    (vec_expand_impl(impl$(v)) ? ((v)->data[(v)->len++] = (val), true) : false)

#define vec_pop(v) \
    (v)->data[--((v)->len)]

#define vec_splice(v, start, count) \
    (vec_splice_impl(impl$(v), start, count), (v)->len -= (count))

#define vec_swapsplice(v, start, count) \
    (vec_swapsplice_impl(impl$(v), start, count), (v)->len -= (count))

#define vec_insert(v, idx, val) (                       \
    {                                                   \
        bool __result = vec_insert_impl(impl$(v), idx); \
        if (__result)                                   \
        {                                               \
            (v)->data[idx] = (val);                     \
            (v)->len++;                                 \
        }                                               \
        __result;                                       \
    })

#define vec_sort(v, fn) \
    qsort((v)->data, (v)->len, sizeof(*(v)->data), fn)

#define vec_swap(v, idx1, idx2) \
    vec_swap_impl(impl$(v), idx1, idx2)

#define vec_truncate(v, len) \
    ((v)->len = (len) < (v)->len ? (len) : (v)->len)

#define vec_clear(v) \
    ((v)->len = 0)

#define vec_at(v, idx) \
    ((v)->data[idx])

#define vec_first(v) \
    (v)->data[0]

#define vec_len(v) \
    ((v)->len)

#define vec_last(v) \
    (v)->data[(v)->len - 1]

#define vec_begin(v) \
    ((v)->data)

#define vec_end(v) \
    ((v)->data + (v)->len)

#define vec_reserve(v, n) \
    vec_reserve_impl(impl$(v), n)

#define vec_compact(v) \
    vec_compact_impl(impl$(v))

#define vec_find(v, val, idx)                      \
    do                                             \
    {                                              \
        for ((idx) = 0; (idx) < (v)->len; (idx)++) \
        {                                          \
            if ((v)->data[(idx)] == (val))         \
                break;                             \
        }                                          \
                                                   \
        if ((idx) == (v)->len)                     \
        {                                          \
            (idx) = -1;                            \
        }                                          \
    } while (0)

#define vec_remove(v, val)           \
    do                               \
    {                                \
        int __idx;                   \
        vec_find(v, val, __idx);     \
                                     \
        if (__idx != -1)             \
        {                            \
            vec_splice(v, __idx, 1); \
        }                            \
    } while (0)

#define vec_reverse(v)                                \
    do                                                \
    {                                                 \
        int __i = (v)->len / 2;                       \
        while (__i--)                                 \
        {                                             \
            vec_swap((v), __i, (v)->len - (__i + 1)); \
        }                                             \
    } while (0)

#define vec_foreach(VAR, SELF) \
    if ((SELF)->len)           \
        for (typeof((SELF)->data) VAR = vec_begin(SELF); VAR != vec_end(SELF); VAR++)

#define vec_foreach_idx(IDX, VAR, SELF)             \
    if ((SELF)->len)                                \
        for (int IDX = 0; IDX < (SELF)->len; IDX++) \
            for (typeof((SELF)->data) VAR = vec_begin(SELF) + IDX; VAR != nullptr; VAR = nullptr)

#define vec_foreach_v(VAR, SELF)                                                  \
    if ((SELF)->len)                                                              \
        for (typeof((SELF)->data + 0) __once##VAR, __it##VAR = vec_begin(SELF); ( \
                 {                                                                \
                     __once##VAR = nullptr;                                       \
                     __it##VAR != vec_end(SELF);                                  \
                 });                                                              \
             __it##VAR++)                                                         \
            for (typeof(*(SELF)->data) VAR = *__it##VAR; __once##VAR == nullptr; __once##VAR = (typeof((SELF)->data))1)
