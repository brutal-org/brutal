#include "kernel/event.h"
#include <brutal-alloc>
#include <brutal-debug>
#include <brutal-ds>

static RwLock _lock = {};
static Vec(EventBinding) _bindings = {};

void event_initialize(void)
{
    vec_init(&_bindings, alloc_global());
}

static EventBinding *event_binding_get(Task const *task, BrEvent event)
{
    vec_foreach(binding, &_bindings)
    {
        if (binding->task == task && br_event_eq(binding->event, event))
        {
            return binding;
        }
    }

    return nullptr;
}

BrResult event_bind(Task const *task, BrEvent event)
{
    rwlock_acquire_write(&_lock);
    EventBinding *existing = event_binding_get(task, event);

    if (existing)
    {
        rwlock_release_write(&_lock);
        return BR_ALREADY_BINDED;
    }

    EventBinding binding = {
        .task = task,
        .event = event,
        .ack = true,
    };

    vec_push(&_bindings, binding);

    rwlock_release_write(&_lock);
    return BR_SUCCESS;
}

static BrResult event_unbind_unlocked(Task const *task, BrEvent event)
{
    for (int i = 0; i < _bindings.len; i++)
    {
        if (_bindings.data[i].task == task &&
            br_event_eq(_bindings.data[i].event, event))
        {
            vec_splice(&_bindings, i, 1);
            return BR_SUCCESS;
        }
    }

    return BR_NOT_BINDED;
}

BrResult event_unbind(Task const *task, BrEvent event)
{
    rwlock_acquire_write(&_lock);

    BrResult result = event_unbind_unlocked(task, event);

    rwlock_release_write(&_lock);

    return result;
}

BrResult event_unbind_all(Task const *task)
{
    rwlock_acquire_write(&_lock);

    for (int i = 0; i < _bindings.len; i++)
    {
        if (_bindings.data[i].task == task)
        {
            vec_splice(&_bindings, i, 1);
        }
    }

    rwlock_release_write(&_lock);

    return BR_SUCCESS;
}

static BrResult event_dispatch(Task const *task, BrEvent event)
{
    return channel_send(
        task->channel,
        nullptr,
        &(BrMsg){
            .from = BR_ADDR_EVENT,
            .event = event,
        },
        0, BR_IPC_SEND);
}

void event_trigger(BrEvent event)
{
    rwlock_acquire_read(&_lock);

    vec_foreach(binding, &_bindings)
    {
        bool expected = true;
        if (br_event_eq(binding->event, event) &&
            atomic_compare_exchange_strong(&binding->ack, &expected, false))
        {
            if (event_dispatch(binding->task, event) == BR_CHANNEL_FULL)
            {
                log$("Channel full, dropping event");
                binding->ack = false;
            }
        }
    }

    rwlock_release_read(&_lock);
}

BrResult event_ack(Task const *task, BrEvent event)
{
    rwlock_acquire_read(&_lock);

    EventBinding *binding = event_binding_get(task, event);

    if (binding == nullptr)
    {
        rwlock_release_read(&_lock);
        return BR_NOT_BINDED;
    }

    binding->ack = true;

    rwlock_release_read(&_lock);

    return BR_SUCCESS;
}
