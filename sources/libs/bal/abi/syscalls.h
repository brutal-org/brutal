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
 * @param[in] message The message to log.
 * @param[in] size The size of the message.
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
 * @param[out] tick The current tick.
 * @param[out] timestamp The current timestamp.
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
 * @param[in] space The space to map the memory into.
 * @param[in] memory The memory to map.
 * @param[in] offset The offset into the memory to map.
 * @param[in] size The size of the memory to map.
 * @param[in] vaddr The virtual address to map the memory to.
 * @param[in] flags The flags to use when mapping the memory.
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
 * @param[in] space The space to unmap the memory from.
 * @param[in] vaddr The virtual address to unmap the memory from.
 * @param[in] size The size of the memory to unmap.
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
 * @param[out] id The id of the object to create.
 * @param[in] type The type of the object to create.
 * @param[out] handle The handle of the object to create.
 * @param[in] task The properties of the task to create.
 * @param[in] memory The properties of the memory to create.
 * @param[in] space The properties of the space to create.
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
 * @param[in] handle The handle to duplicate.
 * @param[out] copy The handle of the duplicated object.
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
 * @param[in] handle The handle of the task to start.
 * @param[in] ip The initial instruction pointer.
 * @param[in] sp The initial stack pointer.
 * @param[in] args The arguments to pass to the task.
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
 * @param[in] handle The handle of the task to exit.
 * @param[in] result The result to return to the caller.
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
 * @param[in] to The address of the task to send/receive to.
 * @param[in] msg The message to send/receive.
 * @param[in] deadline The deadline to wait for the message.
 * @param[in] flags The flags to use when sending/receiving the message.
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
 * @param[in] handle The handle to drop rights from.
 * @param[in] rights The rights to drop.
 * @return BR_SUCCESS if the rights were dropped.
 */
SYSCALL(drop) (BrDropArgs* args);

typedef struct
{
    BrHandle handle;
} BrCloseArgs;

/**
 * @brief Close a handle.
 * @param[in] handle The handle to close.
 * @return BR_SUCCESS if the handle was closed.
 */
SYSCALL(close) (BrCloseArgs* args);

typedef struct
{
    BrEvent event;
} BrBindArgs;

/**
 * @brief Bind an event to a task.
 * @param[in] event The event to bind.
 * @return BR_SUCCESS if the event was bound.
 */
SYSCALL(bind) (BrBindArgs* args);

typedef struct
{
    BrEvent event;
} BrUnbindArgs;

/**
 * @brief Unbind an event from a task.
 * @param[in] event The event to unbind.
 * @return BR_SUCCESS if the event was unbound.
 */
SYSCALL(unbind) (BrUnbindArgs* args);

typedef struct
{
    BrEvent event;
} BrAckArgs;

/**
 * @brief Ack an event.
 * @param[in] event The event to ack.
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
 * @param[in] id The id of the object to inspect.
 * @param[in] handle The handle of the object to inspect.
 * @param[in] type The type of the object to inspect.
 * @param[out] memory The properties of the memory to inspect.
 * @param[out] domain The properties of the domain to inspect.
 * @param[out] space The properties of the space to inspect.
 * @param[out] task The properties of the task to inspect.
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
 * @param[in] port The port to read from.
 * @param[out] data The data read from the port.
 * @param[out] size The size of the data read.
 * @return BR_SUCCESS if the data was read.
 */
SYSCALL(in)(BrIoArgs* args);

/**
 * @brief Write to an IO port.
 * @param[in] port The port to write to.
 * @param[in] data The data to write to the port.
 * @param[in] size The size of the data to write.
 * @return BR_SUCCESS if the data was written.
 */
SYSCALL(out)(BrIoArgs* args);

// clang-format on
