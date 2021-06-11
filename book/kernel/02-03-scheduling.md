## Scheduling

### The context

For an os, scheduling is very important, its need to be optimized, fast, and responsive. We needed a scheduler that support multiple CPU. We also needed a scheduler that did the least task switch possible. For exemple, in a system with 4 cpu and 4 task we may have:

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

### The implementation

If you want you can see the python implementation in `meta/assets/scheduler_test.py`

So in brutal we have (this may change over time) a scheduler state for each task:

```c
struct task_schedule_state
{
    cpu_id_t task_cpu; // only valid if tick_in_cpu is >= 0
    int tick_running; // can be negative !!! when the task hasn't been scheduled in a long time
};
```

**task_cpu** the cpu that the process use.

**tick_running** is the time the process has spent running, or has spent waiting (if negative).

We do a context switch ONLY when the number of waiting process is higher than the number of cpu. 

So we do a loop for each process, we need to count the number of switch we can do, if we have 6 process and 5 cpu, we can do 1 switch, but if we have 11 process and 5 cpu, we can only do 5. So the formula is: 
̀`size_t needed_switch_count = min(cpu_count, process_valid_count - cpu_count);`. Where process_valid_count is the number of running process + the number of waiting process.

Each time we try to switch, we take the most active process and the most waiting process. The most active is replaced with the most waiting. Then we send an ipi to the cpu that need to switch.