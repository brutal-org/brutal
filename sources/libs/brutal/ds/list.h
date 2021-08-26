#pragma once

#include <brutal/alloc/base.h>
#include <brutal/base/cast.h>
#include <brutal/base/keywords.h>
#include <brutal/base/macros.h>
#include <brutal/mem.h>

typedef struct list_impl
{
    void *data;
    struct list_impl *next;
    struct list_impl *prev;
    Alloc *alloc;
} ListImpl;

void list_init_impl(ListImpl *impl, Alloc *alloc);

void list_deinit_impl(ListImpl *impl);

ListImpl *list_insert_impl(ListImpl *impl, int data_size);

void list_remove_impl(ListImpl *head, ListImpl *impl);

#define list(T)           \
    union                 \
    {                     \
        struct            \
        {                 \
            T *data;      \
            void *next;   \
            void *prev;   \
            Alloc *alloc; \
        };                \
        ListImpl _impl;   \
    }

#define list_init(SELF, ALLOC) \
    list_init_impl(impl$(SELF), (ALLOC))

#define list_deinit(SELF) \
    list_deinit_impl(impl$(SELF))

#define list_insert(SELF, ENTRY) (                                             \
    {                                                                          \
        auto res = (typeof(SELF))list_insert_impl(impl$(SELF), sizeof(ENTRY)); \
        (*res->data) = ENTRY;                                                  \
        res;                                                                   \
    })

#define list_loop(VAR, SELF)                                   \
    for (auto VAR = (SELF); VAR; VAR = (typeof(VAR))VAR->next) \
        if (VAR->data != nullptr)

#define list_remove(SELF, ENTRY) \
    list_remove_impl(impl$(SELF), impl$(ENTRY))
