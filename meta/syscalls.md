# Syscalls

- [Syscalls](#syscalls)
  - [Debug](#debug)
    - [br_log](#br_log)
  - [Memory Management](#memory-management)
    - [br_map](#br_map)
    - [br_unmap](#br_unmap)
  - [Object Management](#object-management)
    - [br_create](#br_create)
    - [br_create with task](#br_create-with-task)
    - [br_create with memory](#br_create-with-memory)
    - [br_create with space](#br_create-with-space)
    - [br_create with irq](#br_create-with-irq)
    - [br_close](#br_close)
    - [br_inspect](#br_inspect)
  - [Task Management](#task-management)
    - [br_start](#br_start)
    - [br_exit](#br_exit)
    - [br_drop](#br_drop)
  - [IPC](#ipc)
    - [br_ipc](#br_ipc)
    - [br_bind](#br_bind)
    - [br_unbind](#br_unbind)
    - [br_ack](#br_ack)
  - [Input/Output](#inputoutput)
    - [br_in](#br_in)
    - [br_out](#br_out)

## Debug

### br_log

- Arguments:

  - `char const* message`: Message to be displayed.
  - `size_t size`: Size of the message.

- Result:
  - `BR_SUCCESS`: The message was successfully written to the logs.
  - `BR_BAD_ADDRESS`: The address of the message was not located in user memory.

This syscall display a message in the kernel logs. It's useful when trying to debug something.

## Memory Management

### br_map

- Arguments:

  - `BrHandle space`: Target memory space.
  - `BrHandle memory`: Memory object source.
  - `size_t offset`: Offset from `memory` base.
  - (out/in) `size_t size`: Virtual mapping size.
  - (out/in) `uintptr_t vaddr`: Virtual address.
  - `BrMemoryFlags flags`: Mapping flags:
    - `BR_MEM_READABLE` : Make the memory readable.
    - `BR_MEM_WRITABLE` : Make the memory writable.
    - `BR_MEM_EXECUTABLE` : Make the memory executable.
    - `BR_MEM_USER` : Make the memory only usable by a user process.

- Result:
  - `BR_SUCCESS`: The mapping was successfully created.
  - `BR_BAD_HANDLE`: `memory` or space was not a valid handle.
  - `BR_BAD_ARGUMENT`: The `offset` and `size` where not valid.
  - `BR_OUT_OF_MEMORY`: The syscall wasn't able to allocate memory.

This syscall is used for mapping a `BrHandle` in `space`. The memory will look like:

- Physical:
  - (addr) memory.addr
  - (size) memory.size
- Virtual:
  - (addr) vaddr
  - (size) size

If `vaddr` is equal to 0, the kernel will choose a free virtual address instead.
If `size` is equal to 0, the kernel will use the memory object size instead.

### br_unmap

- Arguments:

  - `BrHandle space`: Address space where the memory will be unmapped.
  - `uintptr_t vaddr`: Start of the range that will be unmapped.
  - `size_t size`: Size of the range that will be unmapped.

- Result:
  - `BR_SUCCESS`: The memory was unmapped successfully.
  - `BR_BAD_HANDLE`: The handle to the space is invalid.
  - `BR_BAD_ADDRESS`: The range is not in the address space.

This syscall unmaps a memory range from an address space.

## Object Management

### br_create

- Arguments:

  - `BrType type`: The object type.
  - (out) `BrId id`: The resulting object id.
  - (out) `BrHandle handle`: The resulting object handle.
  - (out/in) `...`: See br_create object arg.

- Result:
  - `BR_BAD_CAPABILITY`: The task hasn't the `BR_RIGHT_TASK` capability.
  - `...` See br_create object type result.

This syscall is used for making different kernel object (task, memory_space, memory object, irq mapping...).

### br_create with task

- Arguments:

  - `StrFix128 name`: Task name.
  - `BrHandle space`: Task memory space.
  - `BrRight rights`: Task capabilities.
  - `BrTaskFlags flags`: flags.
    - `BR_TASK_USER`: User task.

- Result:
  - `BR_BAD_HANDLE`: `space` is not a valid handle.

### br_create with memory

- Arguments:

  - (out/in) `uintptr_t addr`: Memory object physical addr.
  - `size_t size`: Memory object size.
  - `BrMapFlags flags`: Flags.
    - `BR_MAP_PMM`: Used if you want to specify the physical addr.

- Result:
  - `BR_BAD_CAPABILITY`: The task tried to create a mem object with `BR_MAP_PMM` flag while not having the `BR_RIGHT_PMM` capability.
  - `BR_OUT_OF_MEMORY`: Failed to allocate memory.

Create a memory object. If `addr` is equal to 0, let the kernel choose the physical address.

### br_create with space

- Arguments:

  - `BrSpaceFlags flags`

- Result:
  - `BR_SUCCESS`

### br_close

- Arguments:

  - `BrHandle handle`: handle to close.

- Result:
  - `BR_SUCCESS`
  - `BR_BAD_HANDLE`: The handle was invalid.

Closes an handle.

### br_inspect

- Arguments:

  - `BrHandle handle`: Object handle.
  - (out) `BrInspectArgs info`: Object result info.

- Result:
  - `BR_SUCCESS`
  - `BR_BAD_HANDLE`: `handle` is not a valid handle or the object is a not supported object.

Used for getting information about an object.

## Task Management

### br_start

- Arguments:

  - `BrId task`: Task to start.
  - `uintptr_t ip`: The instruction pointer of the task.
  - `uintptr_t sp`: The stack pointer of the task.
  - `BrTaskArgs args`: The task argument.

- Result:
  - `BR_SUCCESS`: The task was started successfully.
  - `BR_BAD_HANDLE`: The handle to the task was invalid.
  - `BR_BAD_ADDRESS`: The address of `ip`, `sp` or `args` was not located in user memory.

The syscall begins the execution of a task.

### br_exit

- Arguments:

  - `BrId task`: Task to exit.
  - `uintptr_t result`: The exit value of the task.

- Result:
  - `BR_SUCCESS`
  - `BR_BAD_HANDLE`: The task handle was invalid.

This syscall stops the execution of a task.

### br_drop

- Arguments:

  - `BrId task_handle`: Target task.
  - `BrRight rights`: The capabilities to remove.

- Result:
  - `BR_SUCCESS`
  - `BR_BAD_HANDLE`: The task handle was invalid.

This syscall drop one or more task capabilities.

## IPC

### br_ipc

- Arguments:

  - (out/in)`BrId to`: Targe.t
  - (out/in)`BrMsg msg`: Msg to pass.
  - (out/in)`BrDeadline deadline`: Deadline.
  - `BrIpcFlags flags`:
    - `BR_IPC_BLOCK`: Block until the message is received or `deadline` is reached.
    - `BR_IPC_SEND`: Send a message.
    - `BR_IPC_RECV`: Receive a message.

- Result:
  - `BR_SUCCESS`
  - `BR_BAD_ID`: (only when sending) `to` was not a valid Id.
  - `BR_CHANNEL_FULL`: (only when sending) The target process has received too many messages.
  - `BR_BAD_HANDLE`: (only when sending) An handle in `msg` parcel was not valid.
  - `BR_CHANNEL_EMPTY`: (only when receiving) The message channel was empty.

This syscall is used for any IPC action. `deadline` is in tick (or currently in `ms`). If `deadline` is equal to 0 then the process will wait until a message is received.

### br_bind

- Arguments:

  - `BrIrq handle`: Irq handle.
  - `BrIrqFlags flags`:
    - `BR_IRQ_BIND_ONCE`: The irq is unbind once the interrupt is reached.

- Result:
  - `BR_SUCCESS`
  - `BR_BAD_CAPABILITY`: The task has not the `BR_RIGHT_IRQ` capability.
  - `BR_BAD_HANDLE`: `handle` is not a valid handle.

Bind an irq for this current task.

### br_unbind

- Arguments:

  - `BrIrq handle`
  - `BrIrqFlags flags`

- Result:
  - `BR_SUCCESS`
  - `BR_BAD_CAPABILITY`: The task has not the `BR_RIGHT_IRQ` capability.
  - `BR_BAD_HANDLE`: `handle` is not a valid handle.

Unbind an irq for this current task.

### br_ack

- Arguments:

  - `BrIrq handle`
  - `BrIrqFlags flags`

- Result:
  - `BR_SUCCESS`
  - `BR_BAD_CAPABILITY`: The task has not the `BR_RIGHT_IRQ` capability.
  - `BR_BAD_HANDLE`: `handle` is not a valid handle.

Ack an irq for this current task.
Used for getting an interrupt again, after receiving one.

## Input/Output

### br_in

- Arguments:

  - `uint16_t port`: Target port to do an io_in.
  - (out) `uint32_t data`: io_in result.
  - `int size`: io_in size (8bit, 16bit...).

- Result:
  - `BR_SUCCESS`
  - `BR_BAD_CAPABILITY`: the process has not the `BR_RIGHT_IO` capability.

This syscall is used to do `inb`/`inl`/`inw` for x86.

### br_out

- Arguments:

  - `uint16_t port`: Target port to do an io_out.
  - `uint32_t data`: io_out result.
  - `int size`: io_out size (8bit, 16bit...).

- Result:
  - `BR_SUCCESS`
  - `BR_BAD_CAPABILITY`: the process has not the `BR_RIGHT_IO` capability.

This syscall is used to do `outb`/`outl`/`outw` for x86.
