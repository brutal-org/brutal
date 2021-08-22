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

// return the new head if needed
// if you delete all the list entry you will have a NULL returned
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

#define linked_list_init(list, alloc) \
    linked_list_init_impl(impl$(list), (alloc))

#define linked_list_deinit(list) \
    linked_list_deinit_impl(impl$(list))

#define linked_list_insert(list, entry) (                                             \
    {                                                                                 \
        auto res = (typeof(list))linked_list_insert_impl(impl$(list), sizeof(entry)); \
        (*res->data) = entry;                                                         \
        res;                                                                          \
    })

#define linked_list_loop(LIST, ENTRY_NAME)                                                        \
    for (auto ENTRY_NAME = (LIST); ENTRY_NAME; ENTRY_NAME = (typeof(ENTRY_NAME))ENTRY_NAME->next) \
        if (ENTRY_NAME->data != nullptr)

#define linked_list_remove(LIST, ENTRY) \
    linked_list_remove_impl(impl$(LIST), impl$(ENTRY))
