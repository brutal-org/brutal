#pragma once

#include <brutal/alloc.h>
#include <brutal/base/macros.h>
#include <brutal/mem.h>

struct vec_impl
{
    char **data;
    int data_size;

    struct alloc *alloc;
    int *length;
    int *capacity;
};

#define vec_t(T)             \
    struct                   \
    {                        \
        T *data;             \
        struct alloc *alloc; \
        int length;          \
        int capacity;        \
    }

bool vec_expand_impl(struct vec_impl *impl);

bool vec_reserve_impl(struct vec_impl *impl, int n);

bool vec_reserve_po2_impl(struct vec_impl *impl, int n);

bool vec_compact_impl(struct vec_impl *impl);

bool vec_insert_impl(struct vec_impl *impl, int idx);

void vec_splice_impl(struct vec_impl *impl, int start, int count);

void vec_swapsplice_impl(struct vec_impl *impl, int start, int count);

void vec_swap_impl(struct vec_impl *impl, int idx1, int idx2);

#define vec_unpack_impl(v) \
    &(struct vec_impl) { (char **)&(v)->data, sizeof(*(v)->data), (v)->alloc, &(v)->length, &(v)->capacity, }

#define vec_init(v, alloc_) STMT(mem_set((v), 0, sizeof(*(v))); (v)->alloc = (alloc_);)

#define vec_deinit(v) (free((v)->data), vec_init(v))

#define vec_push(v, val) \
    (vec_expand_impl(vec_unpack_impl(v)) ? ((v)->data[(v)->length++] = (val), true) : false)

#define vec_pop(v) (v)->data[--(v)->length]

#define vec_splice(v, start, count) \
    (vec_splice_impl(vec_unpack_impl(v), start, count), (v)->length -= (count))

#define vec_swapsplice(v, start, count) \
    (vec_swapsplice_impl(vec_unpack_impl(v), start, count), (v)->length -= (count))

#define vec_insert(v, idx, val)                                                   \
    (vec_insert_impl(vec_unpack_impl(v), idx) ? -1 : ((v)->data[idx] = (val), 0), \
     (v)->length++, 0)

#define vec_sort(v, fn) qsort((v)->data, (v)->length, sizeof(*(v)->data), fn)

#define vec_swap(v, idx1, idx2) vec_swap_impl(vec_unpack_impl(v), idx1, idx2)

#define vec_truncate(v, len) \
    ((v)->length = (len) < (v)->length ? (len) : (v)->length)

#define vec_clear(v) ((v)->length = 0)

#define vec_first(v) (v)->data[0]

#define vec_last(v) (v)->data[(v)->length - 1]

#define vec_reserve(v, n) vec_reserve_impl(vec_unpack_impl(v), n)

#define vec_compact(v) vec_compact_impl(vec_unpack_impl(v))

#define vec_pusharr(v, arr, count)                                                   \
    STMT(                                                                            \
        int i__, n__ = (count);                                                      \
                                                                                     \
        if (vec_reserve_po2_impl(vec_unpack_impl(v), (v)->length + n__) != 0) break; \
                                                                                     \
        for (i__ = 0; i__ < n__; i__++) {                                            \
            (v)->data[(v)->length++] = (arr)[i__];                                   \
        })

#define vec_extend(v, v2) vec_pusharr((v), (v2)->data, (v2)->length)

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

typedef vec_t(void *) vec_void_t;
typedef vec_t(char *) vec_str_t;
typedef vec_t(int) vec_int_t;
typedef vec_t(char) vec_char_t;
