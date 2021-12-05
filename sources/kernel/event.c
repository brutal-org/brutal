#include <brutal/alloc.h>
#include <brutal/debug.h>
#include <brutal/ds.h>
#include "kernel/event.h"

static Lock _lock = {};
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
    LOCK_RETAINER(&_lock);

    EventBinding *existing = event_binding_get(task, event);

    if (existing)
    {
        return BR_ALREADY_BINDED;
    }

    EventBinding binding = {
        .task = task,
        .event = event,
        .ack = true,
    };

    vec_push(&_bindings, binding);

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
    LOCK_RETAINER(&_lock);
    return event_unbind_unlocked(task, event);
}

BrResult event_unbind_all(Task const *task)
{
    LOCK_RETAINER(&_lock);

    for (int i = 0; i < _bindings.len; i++)
    {
        if (_bindings.data[i].task == task)
        {
            vec_splice(&_bindings, i, 1);
        }
    }

    return BR_SUCCESS;
}

static BrResult event_dispatch(Task const *task, BrEvent event)
{
    return channel_send(
        task->channel,
        nullptr,
        &(BrMsg){
            .from = BR_TASK_IRQ,
            .event = event,
        },
        0, BR_IPC_SEND);
}

void event_trigger(BrEvent event)
{
    LOCK_RETAINER(&_lock);

    vec_foreach(binding, &_bindings)
    {
        if (br_event_eq(binding->event, event) && binding->ack)
        {
            event_dispatch(binding->task, event);
            binding->ack = false;
        }
    }
}

BrResult event_ack(Task const *task, BrEvent event)
{
    LOCK_RETAINER(&_lock);

    EventBinding *binding = event_binding_get(task, event);

    if (binding == nullptr)
    {
        return BR_NOT_BINDED;
    }

    binding->ack = true;

    return BR_SUCCESS;
}
