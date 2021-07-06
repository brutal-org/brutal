#pragma once

#include <brutal/alloc/base.h>
#include <brutal/base/macros.h>
#include <brutal/mem.h>

typedef struct
{
    char *data;
    int length;

    struct alloc *alloc;
    int data_size;
    int capacity;
} VecImpl;

#define Vec(T)          \
    union               \
    {                   \
        struct          \
        {               \
            T *data;    \
            int length; \
        };              \
        VecImpl _impl;  \
    }

void vec_init_impl(VecImpl *impl, int data_size, struct alloc *alloc);

void vec_deinit_impl(VecImpl *impl);

bool vec_expand_impl(VecImpl *impl);

bool vec_reserve_impl(VecImpl *impl, int n);

bool vec_reserve_po2_impl(VecImpl *impl, int n);

bool vec_compact_impl(VecImpl *impl);

bool vec_insert_impl(VecImpl *impl, int idx);

void vec_splice_impl(VecImpl *impl, int start, int count);

void vec_swapsplice_impl(VecImpl *impl, int start, int count);

void vec_swap_impl(VecImpl *impl, int idx1, int idx2);

#define vec_init(v, alloc_) vec_init_impl(&(v)->_impl, sizeof(*(v)->data), alloc_)

#define vec_deinit(v) vec_deinit_impl(&(v)->_impl)

#define vec_push(v, val) \
    (vec_expand_impl(&(v)->_impl) ? ((v)->data[(v)->length++] = (val), true) : false)

#define vec_pop(v) (v)->data[--(v)->length]

#define vec_splice(v, start, count) \
    (vec_splice_impl(&(v)->_impl, start, count), (v)->length -= (count))

#define vec_swapsplice(v, start, count) \
    (vec_swapsplice_impl(&(v)->_impl, start, count), (v)->length -= (count))

#define vec_insert(v, idx, val)                                            \
    (vec_insert_impl(&(v)->_impl, idx) ? -1 : ((v)->data[idx] = (val), 0), \
     (v)->length++, 0)

#define vec_sort(v, fn) qsort((v)->data, (v)->length, sizeof(*(v)->data), fn)

#define vec_swap(v, idx1, idx2) vec_swap_impl(&(v)->_impl, idx1, idx2)

#define Vecruncate(v, len) \
    ((v)->length = (len) < (v)->length ? (len) : (v)->length)

#define vec_clear(v) ((v)->length = 0)

#define vec_first(v) (v)->data[0]

#define vec_last(v) (v)->data[(v)->length - 1]

#define vec_reserve(v, n) vec_reserve_impl(&(v)->_impl, n)

#define vec_compact(v) vec_compact_impl(&(v)->_impl)

#define vec_find(v, val, idx)                         \
    STMT(                                             \
        for ((idx) = 0; (idx) < (v)->length; (idx)++) \
        {                                             \
            if ((v)->data[(idx)] == (val))            \
                break;                                \
        }                                             \
                                                      \
        if ((idx) == (v)->length) {                   \
            (idx) = -1;                               \
        })

#define vec_remove(v, val)           \
    do                               \
    {                                \
        int idx__;                   \
        vec_find(v, val, idx__);     \
                                     \
        if (idx__ != -1)             \
        {                            \
            vec_splice(v, idx__, 1); \
        }                            \
    } while (0)

#define vec_reverse(v)                                   \
    do                                                   \
    {                                                    \
        int i__ = (v)->length / 2;                       \
        while (i__--)                                    \
        {                                                \
            vec_swap((v), i__, (v)->length - (i__ + 1)); \
        }                                                \
    } while (0)

typedef Vec(void *) VecPtr;
typedef Vec(int) VecInt;
typedef Vec(char) VecChar;
