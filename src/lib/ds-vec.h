#pragma once

#include <alloc.h>
#include <base-macros.h>

struct vec_details
{
    char **data;
    int data_size;

    struct alloc *alloc;
    int *length;
    int *capacity;
};

bool vec_details_expand(struct vec_details* details);

bool vec_details_reserve(struct vec_details *details, int n);

bool vec_details_reserve_po2(struct vec_details *details, int n);

bool vec_details_compact(struct vec_details *details);

bool vec_details_insert(struct vec_details *details, int idx);

void vec_details_splice(struct vec_details* details, int start, int count);

void vec_details_swapsplice(struct vec_details* details, int start, int count);

void vec_details_swap(struct vec_details* details, int idx1, int idx2);

#define vec_details_unpack(v) \
    (char **)&(v)->data, &(v)->length, &(v)->capacity, sizeof(*(v)->data)

#define vec_t(T)             \
    struct                   \
    {                        \
        T *data;             \
        struct alloc *alloc; \
        int length;          \
        int capacity;        \
    }

#define vec_init(v, alloc) STMT(memset((v), 0, sizeof(*(v))); (v)->alloc = (alloc))

#define vec_deinit(v) (free((v)->data), vec_init(v))

#define vec_push(v, val)                                                                     \
    (vec_details_expand(vec_details_unpack(v)) ? -1 : ((v)->data[(v)->length++] = (val), 0), \
     0)

#define vec_pop(v) (v)->data[--(v)->length]

#define vec_splice(v, start, count) \
    (vec_details_splice(vec_details_unpack(v), start, count), (v)->length -= (count))

#define vec_swapsplice(v, start, count) \
    (vec_details_swapsplice(vec_details_unpack(v), start, count), (v)->length -= (count))

#define vec_insert(v, idx, val)                                                         \
    (vec_details_insert(vec_details_unpack(v), idx) ? -1 : ((v)->data[idx] = (val), 0), \
     (v)->length++, 0)

#define vec_sort(v, fn) qsort((v)->data, (v)->length, sizeof(*(v)->data), fn)

#define vec_swap(v, idx1, idx2) vec_details_swap(vec_details_unpack(v), idx1, idx2)

#define vec_truncate(v, len) \
    ((v)->length = (len) < (v)->length ? (len) : (v)->length)

#define vec_clear(v) ((v)->length = 0)

#define vec_first(v) (v)->data[0]

#define vec_last(v) (v)->data[(v)->length - 1]

#define vec_reserve(v, n) vec_details_reserve(vec_details_unpack(v), n)

#define vec_compact(v) vec_details_compact(vec_details_unpack(v))

#define vec_pusharr(v, arr, count)                                                         \
    STMT(                                                                                  \
        int i__, n__ = (count);                                                            \
                                                                                           \
        if (vec_details_reserve_po2(vec_details_unpack(v), (v)->length + n__) != 0) break; \
                                                                                           \
        for (i__ = 0; i__ < n__; i__++) {                                                  \
            (v)->data[(v)->length++] = (arr)[i__];                                         \
        })

#define vec_extend(v, v2) vec_pusharr((v), (v2)->data, (v2)->length)

#define vec_find(v, val, idx)                           \
    STMT(                                               \
        for ((idx) = 0; (idx) < (v)->length; (idx)++) { \
            if ((v)->data[(idx)] == (val))              \
                break;                                  \
        }                                               \
                                                        \
        if ((idx) == (v)->length) {                     \
            (idx) = -1;                                 \
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

#define vec_foreach(v, var, iter)                                      \
    if ((v)->length > 0)                                               \
        for ((iter) = 0;                                               \
             (iter) < (v)->length && (((var) = (v)->data[(iter)]), 1); \
             ++(iter))

#define vec_foreach_rev(v, var, iter)  \
    if ((v)->length > 0)               \
        for ((iter) = (v)->length - 1; \
             (iter) >= 0 && (((var) = (v)->data[(iter)]), 1); --(iter))

#define vec_foreach_ptr(v, var, iter)                                   \
    if ((v)->length > 0)                                                \
        for ((iter) = 0;                                                \
             (iter) < (v)->length && (((var) = &(v)->data[(iter)]), 1); \
             ++(iter))

#define vec_foreach_ptr_rev(v, var, iter) \
    if ((v)->length > 0)                  \
        for ((iter) = (v)->length - 1;    \
             (iter) >= 0 && (((var) = &(v)->data[(iter)]), 1); --(iter))

typedef vec_t(void *) vec_void_t;
typedef vec_t(char *) vec_str_t;
typedef vec_t(int) vec_int_t;
typedef vec_t(char) vec_char_t;
typedef vec_t(float) vec_float_t;
typedef vec_t(double) vec_double_t;
