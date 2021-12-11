#pragma once

#include <bal/abi.h>
#include "kernel/domain.h"
#include "kernel/task.h"

typedef struct
{
    Task const *task;
    BrEvent event;
    _Atomic bool ack;
} EventBinding;

void event_initialize(void);

BrResult event_bind(Task const *task, BrEvent event);

BrResult event_unbind(Task const *task, BrEvent event);

BrResult event_unbind_all(Task const *task);

void event_trigger(BrEvent event);

BrResult event_ack(Task const *task, BrEvent event);
