#pragma once

#include <bal/abi/types.h>

// clang-format off

#ifdef __kernel__
#    define SYSCALL(NAME) BrResult sys_##NAME
#else
#    define SYSCALL(NAME) BrResult br_##NAME
SYSCALL(syscall) (BrSyscall syscall, BrArg arg1, BrArg arg2, BrArg arg3, BrArg arg4, BrArg arg5);
#endif

typedef struct
{
    char const *message;
    size_t size;
} BrLogArgs;

/**
 * @brief Log a message to the kernel log.
 * @param message The message to log. (in)
 * @param size The size of the message. (in)
 * @return BR_SUCCESS if the message was logged.
 */
SYSCALL(log) (BrLogArgs* args);

typedef struct
{
    BrTick tick;
    BrTimeStamp timestamp;
} BrNowArgs;

/**
 * @brief Get the current time.
 * @param tick The current tick. (out)
 * @param timestamp The current timestamp. (out)
 */
SYSCALL(now) (BrNowArgs* args);

typedef struct
{
    BrHandle space;
    BrHandle memory;

    size_t offset;
    size_t size;
    uintptr_t vaddr;
    BrMemoryFlags flags;
} BrMapArgs;

/**
 * @brief Map a memory region into a space.
 * @param space The space to map the memory into. (in)
 * @param memory The memory to map. (in)
 * @param offset The offset into the memory to map. (in)
 * @param size The size of the memory to map. (in)
 * @param vaddr The virtual address to map the memory to. (in)
 * @param flags The flags to use when mapping the memory. (in)
 * @return BR_SUCCESS if the memory was mapped.
 */
SYSCALL(map) (BrMapArgs* args);

typedef struct
{
    BrHandle space;

    uintptr_t vaddr;
    size_t size;
} BrUnmapArgs;

/**
 * @brief Unmap a memory region from a space.
 * @param space The space to unmap the memory from. (in)
 * @param vaddr The virtual address to unmap the memory from. (in)
 * @param size The size of the memory to unmap. (in)
 * @return BR_SUCCESS if the memory was unmapped.
 */
SYSCALL(unmap) (BrUnmapArgs* args);

typedef struct
{
    BrHandle space;
    BrRight rights;
    BrTaskFlags flags;
} BrTaskProps;

typedef struct
{
    uintptr_t addr;
    size_t size;
    BrMemoryFlags flags;
} BrMemoryProps;

typedef struct
{
    BrSpaceFlags flags;
} BrSpaceProps;

typedef struct
{
    BrId id;
    BrType type;
    BrHandle handle;

    union
    {
        BrTaskProps task;
        BrMemoryProps memory;
        BrSpaceProps space;
    };
} BrCreateArgs;

/**
 * @brief Create a new kernel object.
 * @param id The id of the object to create. (out)
 * @param type The type of the object to create. (in)
 * @param handle The handle of the object to create. (out)
 * @param task The properties of the task to create. (in)
 * @param memory The properties of the memory to create. (in)
 * @param space The properties of the space to create. (in)
 * @return BR_SUCCESS if the object was created.
 */
SYSCALL(create) (BrCreateArgs* args);

typedef struct
{
    BrHandle handle;
    BrHandle copy;
} BrDupArgs;

/**
 * @brief Duplicate an handle to kernel object.
 * @param handle The handle to duplicate. (in)
 * @param copy The handle of the duplicated object. (out)
 * @return BR_SUCCESS if the object was duplicated.
 */
SYSCALL(dup) (BrDupArgs* args);

typedef struct
{
    BrHandle handle;
    uintptr_t ip;
    uintptr_t sp;
    BrTaskArgs args;
} BrStartArgs;

/**
 * @brief Start a task.
 * @param handle The handle of the task to start. (in)
 * @param ip The initial instruction pointer. (in)
 * @param sp The initial stack pointer. (in)
 * @param args The arguments to pass to the task. (in)
 * @return BR_SUCCESS if the task was started.
 */
SYSCALL(start) (BrStartArgs* args);

typedef struct
{
    BrHandle handle;
    uintptr_t result;
} BrExitArgs;

/**
 * @brief Exit a task.
 * @param handle The handle of the task to exit. (in)
 * @param result The result to return to the caller. (in)
 * @return BR_SUCCESS if the task was exited.
 */
SYSCALL(exit) (BrExitArgs* args);

#define BR_IPC_BLOCK ((BrIpcFlags)(1 << 0))
#define BR_IPC_SEND ((BrIpcFlags)(1 << 1))
#define BR_IPC_RECV ((BrIpcFlags)(1 << 2))

typedef uint32_t BrIpcFlags;

typedef struct
{
    BrAddr to;
    BrMsg msg;
    BrDeadline deadline;
    BrIpcFlags flags;
} BrIpcArgs;

/**
 * @brief Send/receive a message to/from another task.
 * @param to The address of the task to send/receive to. (in)
 * @param msg The message to send/receive. (in)
 * @param deadline The deadline to wait for the message. (in)
 * @param flags The flags to use when sending/receiving the message. (in)
 * @return BR_SUCCESS if the message was sent/received.
 */
SYSCALL(ipc) (BrIpcArgs* args);

typedef struct
{
    BrHandle handle;
    BrRight rights;
} BrDropArgs;

/**
 * @brief Drop rights from a handle.
 * @param handle The handle to drop rights from. (in)
 * @param rights The rights to drop. (in)
 * @return BR_SUCCESS if the rights were dropped.
 */
SYSCALL(drop) (BrDropArgs* args);

typedef struct
{
    BrHandle handle;
} BrCloseArgs;

/**
 * @brief Close a handle.
 * @param handle The handle to close. (in)
 * @return BR_SUCCESS if the handle was closed.
 */
SYSCALL(close) (BrCloseArgs* args);

typedef struct
{
    BrEvent event;
} BrBindArgs;

/**
 * @brief Bind an event to a task.
 * @param event The event to bind. (in)
 * @return BR_SUCCESS if the event was bound.
 */
SYSCALL(bind) (BrBindArgs* args);

typedef struct
{
    BrEvent event;
} BrUnbindArgs;

/**
 * @brief Unbind an event from a task.
 * @param event The event to unbind. (in)
 * @return BR_SUCCESS if the event was unbound.
 */
SYSCALL(unbind) (BrUnbindArgs* args);

typedef struct
{
    BrEvent event;
} BrAckArgs;

/**
 * @brief Ack an event.
 * @param event The event to ack. (in)
 * @return BR_SUCCESS if the event was acked.
 */
SYSCALL(ack) (BrAckArgs* args);

typedef struct
{
    BrRight rights;
    bool stopped;
    bool started;
    bool blocked;
} BrTaskInfos;

typedef struct
{
    size_t domain_object_count;
} BrDomainInfos;

typedef struct
{
    USizeRange range;
} BrMemoryInfos;

typedef struct
{
    BrSpaceFlags flags;
} BrSpaceInfos;

typedef struct
{
    BrId id;
    BrHandle handle;
    BrType type;

    union
    {
        BrMemoryInfos memory;
        BrDomainInfos domain;
        BrSpaceInfos space;
        BrTaskInfos task;
    };
} BrInspectArgs;

/**
 * @brief Inspect a kernel object.
 * @param id The id of the object to inspect. (in)
 * @param handle The handle of the object to inspect. (in)
 * @param type The type of the object to inspect. (in)
 * @param memory The properties of the memory to inspect. (out)
 * @param domain The properties of the domain to inspect. (out)
 * @param space The properties of the space to inspect. (out)
 * @param task The properties of the task to inspect. (out)
 * @return BR_SUCCESS if the object was inspected.
 */
SYSCALL(inspect)(BrInspectArgs* args);

typedef struct
{
    uint16_t port;
    uint32_t data;
    int size;
}BrIoArgs;

/**
 * @brief Read from an IO port.
 * @param port The port to read from. (in)
 * @param data The data read from the port. (out)
 * @param size The size of the data read. (out)
 * @return BR_SUCCESS if the data was read.
 */
SYSCALL(in)(BrIoArgs* args);

/**
 * @brief Write to an IO port.
 * @param port The port to write to. (in)
 * @param data The data to write to the port. (in)
 * @param size The size of the data to write. (in)
 * @return BR_SUCCESS if the data was written.
 */
SYSCALL(out)(BrIoArgs* args);

// clang-format on
