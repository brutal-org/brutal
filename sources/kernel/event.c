#include <brutal/alloc.h>
#include <brutal/debug.h>
#include <brutal/ds.h>
#include "kernel/event.h"

static Lock lock = {};
static Vec(EventBinding) bindings = {};

void event_initialize(void)
{
    vec_init(&bindings, alloc_global());
}

static EventBinding *event_binding_get(Task const *task, BrEvent event)
{
    for (int i = 0; i < bindings.len; ++i)
    {
        EventBinding *binding = bindings.data + i;
        if (binding->task == task && br_event_eq(binding->event, event))
        {
            return binding;
        }
    }
    return NULL;
}

BrResult event_bind(Task const *task, BrEvent event)
{
    LOCK_RETAINER(&lock);

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

    vec_push(&bindings, binding);

    return BR_SUCCESS;
}

static BrResult event_unbind_unlocked(Task const *task, BrEvent event)
{
    for (int i = 0; i < bindings.len; i++)
    {
        if (bindings.data[i].task == task &&
            br_event_eq(bindings.data[i].event, event))
        {
            vec_splice(&bindings, i, 1);
            return BR_SUCCESS;
        }
    }

    return BR_NOT_BINDED;
}

BrResult event_unbind(Task const *task, BrEvent event)
{
    LOCK_RETAINER(&lock);
    return event_unbind_unlocked(task, event);
}

BrResult event_unbind_all(Task const *task)
{
    LOCK_RETAINER(&lock);

    for (int i = 0; i < bindings.len; i++)
    {
        if (bindings.data[i].task == task)
        {
            vec_splice(&bindings, i, 1);
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
    LOCK_RETAINER(&lock);

    vec_foreach(binding, &bindings)
    {
        if (br_event_eq(binding.event, event) && binding.ack)
        {
            event_dispatch(binding.task, event);
            binding.ack = false;
        }
    }
}

BrResult event_ack(Task const *task, BrEvent event)
{
    LOCK_RETAINER(&lock);

    EventBinding *binding = event_binding_get(task, event);

    if (binding == nullptr)
    {
        return BR_NOT_BINDED;
    }

    binding->ack = true;

    return BR_SUCCESS;
}
