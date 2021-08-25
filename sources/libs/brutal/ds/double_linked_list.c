#include "double_linked_list.h"

static void linked_list_deinit_head(LinkedListImpl *entry)
{
    if (entry->data)
    {
        alloc_release(entry->alloc, entry->data);
    }
    entry->data = nullptr;
}

static void linked_list_deinit_entry(LinkedListImpl *entry)
{
    if (entry->data)
    {
        alloc_release(entry->alloc, entry->data);
    }
    alloc_release(entry->alloc, entry);
}

void linked_list_init_impl(LinkedListImpl *impl, Alloc *alloc)
{
    impl->alloc = alloc;
    impl->data = nullptr;
    impl->next = nullptr;
    impl->prev = nullptr;
};

void linked_list_deinit_impl(LinkedListImpl *impl)
{
    size_t i = 0;

    LinkedListImpl *entry = impl->next;

    while (entry != nullptr)
    {
        LinkedListImpl *next = entry->next;

        linked_list_deinit_entry(entry);

        entry = next;

        i++;
    }
}

LinkedListImpl *linked_list_insert_impl(LinkedListImpl *impl, int data_size)
{
    LinkedListImpl *final = alloc_make(impl->alloc, LinkedListImpl);
    LinkedListImpl *current = impl;

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

void linked_list_remove_impl(LinkedListImpl *head, LinkedListImpl *impl)
{

    // impl -> [NEXT]
    if (impl == head)
    {
        linked_list_deinit_head(head);
    }

    LinkedListImpl *self = impl;
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

    linked_list_deinit_entry(self);
}
