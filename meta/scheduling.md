# Scheduling

## The Context

For an os, scheduling is very important, its need to be optimized, fast, and responsive. We needed a scheduler that support multiple CPU. We also needed a scheduler that did the least task switch possible. For example, in a system with 4 cpu and 4 task we may have:

| cpu    | 1   | 2   | 3   | 4   |
| ------ | --- | --- | --- | --- |
| tick 1 | A   | B   | C   | D   |
| tick 2 | A   | B   | C   | D   |
| tick 3 | A   | B   | C   | D   |
| tick 4 | A   | B   | C   | D   |
| tick 5 | A   | B   | C   | D   |

(no context switch)

That mean that we schedule ONLY when the number of running processes is higher than the number of cpus.

but if we have 5 process with 4 cpu we may have:

| cpu    | 1   | 2   | 3   | 4   |
| ------ | --- | --- | --- | --- |
| tick 1 | A   | B   | C   | D   |
| tick 2 | E   | A   | B   | C   |
| tick 3 | D   | E   | A   | B   |
| tick 4 | C   | D   | E   | A   |
| tick 5 | B   | C   | D   | E   |

This is bad and ineffective, yes every process has runned 4 times but every process has a context switch. We want the least context switch possible. Here is the solution:

| cpu    | 1   | 2   | 3   | 4   |
| ------ | --- | --- | --- | --- |
| tick 1 | E   | B   | C   | D   |
| tick 2 | A   | E   | C   | D   |
| tick 3 | A   | B   | E   | D   |
| tick 4 | A   | B   | C   | E   |
| tick 5 | A   | B   | C   | D   |

Here every process is called 4 times. A B C and D have the least amount of context switch. Only process E change of cpu frequently. This is really good but not perfect. The fact that E change of cpu is bad. Certainly this method is effective but not equal between processes. The E process will have more switch than the others.

Here is the solution:

| cpu    | 1   | 2   | 3   | 4   |
| ------ | --- | --- | --- | --- |
| tick 1 | A   | C   | D   | E   |
| tick 2 | A   | B   | D   | E   |
| tick 3 | A   | B   | C   | E   |
| tick 4 | A   | B   | C   | D   |
| tick 5 | E   | B   | C   | D   |
| tick 6 | E   | A   | C   | D   |
| tick 7 | E   | A   | B   | D   |
| tick 8 | E   | A   | B   | C   |
| tick 9 | D   | A   | B   | C   |

this is way better, but here each process waits 1 tick and stays for 4 tick in the cpu. Each process here are on the same level.

## The Implementation

So in brutal we have (this may change over time) a scheduler state for each task:

```c
struct TaskSchedule
{
    int cpu;
    int tick_start;
    int tick_end;
    bool is_currently_executed;

};
```

- **cpu** the cpu that the process use.
- **tick_start** the tick where the process started executing
- **tick_end** the tick where the process stopped executing
- **is_currently_executed** if the process is executed or not

But we have some problems: What happens if a process is now runnable ? or if we have 2 process and 4 cpu and 1 turning runnable ?

That's why we have 2 part of the scheduler:

- when the number of running processes is lower than the number of cpu
- when the number of running processes is higher than the number of cpu

## When The Number Of Running Processes Is Lower Than The Number Of Cpu

If the number of processes is lower than the number of cpu we don't need to switch context. But if we have 1 process that started running and it does not execute code we need to assign him a cpu. That's why we have 'idle cpu'. Lazy cpu are cpu that don't do anything for the moment. For example if we have 3 process and 5 core, we have 2 idle cpu. So if a cpu is idle and there is a process waiting to run, we give the process to the idle cpu. For cpu that are already running a process we just put the same process as the next process.

## When The Number Of Running Processes Is Higher Than The Number Of Cpu

If the number of processes is higher than the number of cpu we need to switch X process. Were X is equal to:

```c
size_t schedule_count = m_min(cpu_count(), running - cpu_count());
```

(where running is the number of running process)

We can't switch higher than the cpu_count (we can't switch for 6 process using 5 cpu) and we can't switch more process than there is cpu (we can't switch for 1 process for 5 cpu).

For each `schedule_count` we get the process that has run the longest and replace it with the process that waited the longest. But we can still have the case where we have a idle cpu. For example if a process is deleted or sleeping so when we get the most waiting process, we check if there is no idle cpu. If there is one, put the most waiting process in the idle cpu. If there is not replace the most running process with the most waiting process.

## CPU Cache Is Important

The problem of interrupt is that they are really heavy, they invalidate the cache, they can take a lot of cpu cycles... So we need to send them only when they are needed. That mean that if a cpu don't need to switch to another process we don't send him an interrupt (if a process next task is equal to the process current task)

## Task Switch

So for each cpu that need a switch the cpu 0 send him a IPI (inter process interrupt) number 100. The process set the current process to the next process, and do a context switch.
