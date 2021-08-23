#pragma once

#include <brutal/alloc/base.h>
#include <brutal/base/cast.h>
#include <brutal/base/keywords.h>
#include <brutal/base/macros.h>
#include <brutal/mem.h>

typedef struct linked_list_impl_entry_t LinkedListImpl;

typedef struct linked_list_impl_entry_t
{
    void *data;
    LinkedListImpl *next;
    LinkedListImpl *prev;
    Alloc *alloc;
} LinkedListImpl;

void linked_list_init_impl(LinkedListImpl *impl, Alloc *alloc);

void linked_list_deinit_impl(LinkedListImpl *impl);

LinkedListImpl *linked_list_insert_impl(LinkedListImpl *impl, int data_size);

void linked_list_remove_impl(LinkedListImpl *head, LinkedListImpl *impl);

#define linked_list(T)        \
    union                     \
    {                         \
        struct                \
        {                     \
            T *data;          \
            void *next;       \
            void *prev;       \
            Alloc *alloc;     \
        };                    \
        LinkedListImpl _impl; \
    }

#define linked_list_init(SELF, ALLOC) \
    linked_list_init_impl(impl$(SELF), (ALLOC))

#define linked_list_deinit(SELF) \
    linked_list_deinit_impl(impl$(SELF))

#define linked_list_insert(SELF, ENTRY) (                                             \
    {                                                                                 \
        auto res = (typeof(SELF))linked_list_insert_impl(impl$(SELF), sizeof(ENTRY)); \
        (*res->data) = ENTRY;                                                         \
        res;                                                                          \
    })

#define linked_list_loop(VAR, SELF)                            \
    for (auto VAR = (SELF); VAR; VAR = (typeof(VAR))VAR->next) \
        if (VAR->data != nullptr)

#define linked_list_remove(SELF, ENTRY) \
    linked_list_remove_impl(impl$(SELF), impl$(ENTRY))
