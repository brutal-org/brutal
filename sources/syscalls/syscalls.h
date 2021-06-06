#pragma once

typedef enum
{
    BR_SYS_GROUP,
    BR_SYS_TASK,
    BR_SYS_KILL,
    BR_SYS_WAIT,
    BR_SYS_IPC,
    BR_SYS_MAP,
    BR_SYS_COMMIT,
    BR_SYS_DECOMMIT,
    BR_SYS_UNMAP,
    BR_SYS_DROP,
} br_syscall_t;

void br_sys_group();

// Create a new task task by fork or clone
void br_sys_task();

// Kill an existing task
void br_sys_kill();

// Wait for a specific event
void br_sys_wait();

// Send a massage to another process
void br_sys_ipc();

// Map a memory range
void br_sys_map();

void br_sys_commit();

void br_sys_decommit();

// Unmap a memory range
void br_sys_unmap();

// Drop a capability
void br_sys_drop();
