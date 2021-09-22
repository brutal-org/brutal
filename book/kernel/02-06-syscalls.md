## Syscalls

The kernel syscalls are defined in the file `librairies/syscalls/syscalls.h`.

> Note: these syscalls may change during the development of the kernel.

### br_log

- Arguments:
  - `char const* message`: Message to be displayed.
  - `size_t size`: Size of the message.

- Result:
  - `BR_SUCCESS`: The message was successfully written to the logs.
  - `BR_BAD_ADDRESS`: The address of the message was not located in user memory.

This syscall display a message in the kernel logs. It's useful when trying to debug something.

### br_map

- Arguments:
  - `BrSpace space`: Target memory space.
  - `BrMemObj mem_obj`: Memory object source.
  - `size_t offset`: Offset from `mem_obj` base.
  - (out/in) `size_t size`: Virtual mapping size.
  - (out/in) `uintptr_t vaddr`: Virtual address.
  - `BrMemFlags flags`: Mapping flags:
    - `BR_MEM_READABLE` : Make the memory readable.
    - `BR_MEM_WRITABLE` : Make the memory writable.
    - `BR_MEM_EXECUTABLE` : Make the memory executable.
    - `BR_MEM_USER` : Make the memory only usable by a user process.

- Result:
  - `BR_SUCCESS`: The mapping was successfully created.
  - `BR_BAD_HANDLE`: `mem_obj` or space was not a valid handle.
  - `BR_BAD_ARGUMENT`: The `offset` and  `size` where not valid.
  - `BR_OUT_OF_MEMORY`: The syscall wasn't able to allocate memory.

This syscall is used for mapping a `BrMemObj` in `space`. The memory will look like:

- Physical:
  - (addr) mem_obj.addr
  - (size) mem_obj.size
- Virtual:
  - (addr) vaddr
  - (size) size

If `vaddr` is equal to 0, the kernel will choose a free virtual address instead.
If `size` is equal to 0, the kernel will use the memory object size instead.

### br_unmap

- Arguments:
  - `BrSpace space`: Address space where the memory will be unmapped.
  - `uintptr_t vaddr`: Start of the range that will be unmapped.
  - `size_t size`: Size of the range that will be unmapped.

- Result:
  - `BR_SUCCESS`: The memory was unmapped successfully.
  - `BR_BAD_HANDLE`: The handle to the space is invalid.
  - `BR_BAD_ADDRESS`: The range is not in the address space.

This syscall unmaps a memory range from an address space.

### br_create

- Arguments:  
  - `BrObjectType type`: The object type.
  - (out) `BrId id`: The resulting object id.
  - (out) `BrHandle handle`: The resulting object handle.
  - (out/in) `...`: See br_create object arg.

- Result:
  - `BR_BAD_CAPABILITY`: The task hasn't the `BR_CAP_TASK` capability.
  - `...` See br_create object type result.

This syscall is used for making different kernel object (task, memory_space, memory object, irq mapping...).

#### br_create with task

- Arguments:
  - `StrFix128 name`: Task name.
  - `BrSpace space`: Task memory space.
  - `BrCap caps`: Task capabilities.
  - `BrTaskFlags flags`: flags.
    - `BR_TASK_USER`: User task.

- Result:
  - `BR_BAD_HANDLE`: `space` is not a valid handle.

#### br_create with mem_obj

- Arguments:
  - (out/in) `uintptr_t addr`: Memory object physical addr.
  - `size_t size`: Memory object size.
  - `BrMemObjFlags flags`: Flags.
    - `BR_MEM_OBJ_PMM`: Used if you want to specify the physical addr.

- Result:
  - `BR_BAD_CAPABILITY`: The task tried to create a mem object with `BR_MEM_OBJ_PMM` flag while not having the `BR_CAP_PMM` capability.
  - `BR_OUT_OF_MEMORY`: Failed to allocate memory.

Create a memory object. If `addr` is equal to 0, let the kernel choose the physical address.

#### br_create with space

- Arguments:
  - `BrSpaceFlags flags`

- Result:
  - `BR_SUCCESS`

#### br_create with irq

- Arguments:
  - `BrIrqId irq`: Irq to map.

- Result:
  - `BR_SUCCESS`
  - `BR_BAD_CAPABILITY`: The current process has not the `BR_CAP_IRQ` capability.

### br_start

- Arguments:
  - `BrTask task`: Task to start.
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
  - `BrTask task`: Task to exit.
  - `uintptr_t exit_value`: The exit value of the task.

- Result:
  - `BR_SUCCESS`
  - `BR_BAD_HANDLE`: The task handle was invalid.

This syscall stops the execution of a task.

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
  - `BR_BAD_HANDLE`: (only when sending) An handle in `msg` envelope was not valid.
  - `BR_CHANNEL_EMPTY`: (only when receiving) The message channel was empty.

This syscall is used for any IPC action. `deadline` is in tick (or currently in `ms`). If `deadline` is equal to 0 then the process will wait until a message is received.

### br_drop

- Arguments:
  - `BrTask task_handle`: Target task.
  - `BrCap cap`: The capability to remove.

- Result:
  - `BR_SUCCESS`
  - `BR_BAD_HANDLE`: The task handle was invalid.

This syscall drop one or more task capabilities.

### br_close

- Arguments:
  - `BrHandle handle`: handle to close.

- Result:
  - `BR_SUCCESS`
  - `BR_BAD_HANDLE`: The handle was invalid.

This syscall closes a handle.

### br_bind

- Arguments:
  - `BrIrq handle`: Irq handle.
  - `BrIrqFlags flags`:
    - `BR_IRQ_BIND_ONCE`: The irq is unbind once the interrupt is reached.

- Result:
  - `BR_SUCCESS`
  - `BR_BAD_CAPABILITY`: The task has not the `BR_CAP_IRQ` capability.
  - `BR_BAD_HANDLE`: `handle` is not a valid handle.

Bind an irq for this current task.

### br_unbind

- Arguments:
  - `BrIrq handle`
  - `BrIrqFlags flags`

- Result:
  - `BR_SUCCESS`
  - `BR_BAD_CAPABILITY`: The task has not the `BR_CAP_IRQ` capability.
  - `BR_BAD_HANDLE`: `handle` is not a valid handle.

Unbind an irq for this current task.

### br_ack

- Arguments:
  - `BrIrq handle`
  - `BrIrqFlags flags`

- Result:
  - `BR_SUCCESS`
  - `BR_BAD_CAPABILITY`: The task has not the `BR_CAP_IRQ` capability.
  - `BR_BAD_HANDLE`: `handle` is not a valid handle.

Ack an irq for this current task.
Used for getting an interrupt again, after receiving one.

### br_stat

- Arguments:
  - `BrHandle handle`: Object handle.
  - (out) `BrHandleInfo info`: Object result info.

- Result:
  - `BR_SUCCESS`
  - `BR_BAD_HANDLE`: `handle` is not a valid handle or the object is a not supported object.

Used for getting information about an object.

### br_in

- Arguments:
  - `uint16_t port`: Target port to do an io_in.
  - (out) `uint32_t data`: io_in result.
  - `int size`: io_in size (8bit, 16bit...).

- Result:
  - `BR_SUCCESS`
  - `BR_BAD_CAPABILITY`: the process has not the `BR_CAP_IO` capability.

This syscall is used to do `inb`/`inl`/`inw` for x86.

### br_out

- Arguments:
  - `uint16_t port`: Target port to do an io_out.
  - `uint32_t data`: io_out result.
  - `int size`: io_out size (8bit, 16bit...).

- Result:
  - `BR_SUCCESS`
  - `BR_BAD_CAPABILITY`: the process has not the `BR_CAP_IO` capability.

This syscall is used to do `outb`/`outl`/`outw` for x86.
