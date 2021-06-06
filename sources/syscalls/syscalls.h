#pragma once

typedef enum
{
    HJ_SYS_GROUP,
    HJ_SYS_TASK,
    HJ_SYS_KILL,
    HJ_SYS_WAIT,
    HJ_SYS_IPC,
    HJ_SYS_MAP,
    HJ_SYS_COMMIT,
    HJ_SYS_DECOMMIT,
    HJ_SYS_UNMAP,
    HJ_SYS_DROP,
} hj_syscall_t;

void hj_sys_group();

// Create a new task task by fork or clone
void hj_sys_task();

// Kill an existing task
void hj_sys_kill();

// Wait for a specific event
void hj_sys_wait();

// Send a massage to another process
void hj_sys_ipc();

// Map a memory range
void hj_sys_map();

void hj_sys_commit();

void hj_sys_decommit();

// Unmap a memory range
void hj_sys_unmap();

// Drop a capability
void hj_sys_drop();
