#include "list.h"

static void list_deinit_head(ListImpl *entry)
{
    if (entry->data)
    {
        alloc_release(entry->alloc, entry->data);
    }
    entry->data = nullptr;
}

static void list_deinit_entry(ListImpl *entry)
{
    if (entry->data)
    {
        alloc_release(entry->alloc, entry->data);
    }
    alloc_release(entry->alloc, entry);
}

void list_init_impl(ListImpl *impl, Alloc *alloc)
{
    impl->alloc = alloc;
    impl->data = nullptr;
    impl->next = nullptr;
    impl->prev = nullptr;
};

void list_deinit_impl(ListImpl *impl)
{
    size_t i = 0;

    ListImpl *entry = impl->next;

    while (entry != nullptr)
    {
        ListImpl *next = entry->next;

        list_deinit_entry(entry);

        entry = next;

        i++;
    }
}

ListImpl *list_insert_impl(ListImpl *impl, int data_size)
{
    ListImpl *final = alloc_make(impl->alloc, ListImpl);
    ListImpl *current = impl;

    while (current->next != nullptr)
    {
        current = current->next;
    }

    current->next = final;
    final->next = nullptr;
    final->prev = current;

    final->data = alloc_malloc(impl->alloc, data_size);

    return final;
}

void list_remove_impl(ListImpl *head, ListImpl *impl)
{

    // impl -> [NEXT]
    if (impl == head)
    {
        list_deinit_head(head);
    }

    ListImpl *self = impl;
    if (impl->prev && impl->next)
    {
        // [PREV] -> impl -> [NEXT]
        impl->prev->next = impl->next;
        impl->next->prev = impl->prev;
    }
    else if (impl->prev)
    {
        // [PREV] -> impl -> NULL
        impl->prev->next = nullptr;
    }

    list_deinit_entry(self);
}
