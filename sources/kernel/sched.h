#pragma once

#include "kernel/cpu.h"
#include "kernel/task.h"

void sched_initialize(void);

Tick sched_tick(void);

void sched_start(Task *task, uintptr_t ip, uintptr_t sp, BrTaskArgs args);

void sched_stop(Task *task, uintptr_t result);

void sched_crash(Task *task);

void sched_finalize(void);

BrResult sched_block(Blocker blocker);

Tick sched_deadline(BrTimeout timeout);

void sched_yield(void);

void sched_next(Task *task, Cpu *cpu);

void sched_current(Task *task, Cpu *cpu);

void sched_schedule(void);

void sched_switch(void);
