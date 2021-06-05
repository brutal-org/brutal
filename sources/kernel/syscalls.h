#pragma once

// Create a new task task by fork or clone
void sys_task();

// Kill an existing task
void sys_kill();

// Wait for a specific event
void sys_wait();

// Send a massage to another process
void sys_ipc();

// Map a memory range
void sys_map();

void sys_commit();

void sys_decommit();

// Unmap a memory range
void sys_unmap();

// Drop a capability
void sys_drop();
