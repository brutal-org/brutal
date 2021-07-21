
## syscalls 

The kernel syscalls are defined in the file `librairies/syscalls/syscalls.h`.

> Note: these syscalls may change during the development of the kernel.

### br_noop

- Arguments: none.

The syscall does nothing and always returns `BR_SUCCESS`.

### br_log

- Arguments: 
  - `const char* message`: message to be displayed. 
  - `size_t size`: size of the message.

- Result:
  - `BR_SUCCESS`: The message was successfully written to the logs.
  - `BR_BAD_ADDRESS`: The address of the message was not located in user memory.

This syscall display a message in the kernel logs. It's useful when trying to debug something.

### br_space

- Arguments:
  - (out) `BrSpace* space`: handle of the space if created successfully, or `BR_SPACE_ERROR`.
  - `BrSpaceFlags flags`.

- Result:
  - `BR_SUCCESS`: The `space` successfully created.
  - `BR_BAD_ADDRESS`: The address of the space was not located in user memory.
  - `BR_BAD_CAPABILITY`: The task doesn't have the required capabilities.
  - `BR_OUT_OF_MEMORY`: There was not enough memory to complete the operation.

This syscall creates a new address space. If there is an error during the creation of the space, it may set `space` to `BR_SPACE_ERROR`.

### br_mobj

- Arguments: 
  - (out) `BrMObj* mobj`: handle of the mobj if created successfully, or `BR_MOBJ_ERROR`.
  - `uintptr_t addr`: physical address of the MOBJ if `BR_MOBJ_PMM` is set.
  - `size_t size`: size of the MOBJ. 
  - `BrMObjFlags flags`.

- Result:
  - `BR_SUCCESS`: The mobj was created successfully.
  - `BR_BAD_ADDRESS`: The address of the mobj was not located in user memory.
  - `BR_BAD_CAPABILITY`: The task doesn't have the required capabilities.
  - `BR_OUT_OF_MEMORY`: There was not enough memory to complete the operation.

This syscall creates a new virtual memory object that can be passed across multiples address spaces. It returns the created object in `mobj` or `BR_MOBJ_ERROR` if there is an error during the creation of the MOBJ. The physical address may be `addr` if `BR_MOBJ_PMM` flag is set. 

### br_map

- Arguments:
  - `BrSpace space`: target address where the `mobj` will be mapped.
  - `BrMobj mobj`: the MOBJ that will be mapped into the address space.
  - `uintptr_t vaddr`: the address where the MOBJ may be mapped.
  - `BrMemFlags flags`: access flags

- Result:
  - `BR_SUCCESS`: The mapping was created successfully.
  - `BR_BAD_HANDLE`: The handle of the `space` or `mobj` was invalid.
  - `BR_OUT_OF_MEMORY`: There was not enough memory to complete the operation.

This syscall map a Mobj into a target address space.

### br_alloc

- Arguments:
  - `BrSpace space`: Address space where the mobj will be mapped.
  - `BrMObj mobj`: MOBJ that will be mapped into the address space.
  - (out) `uintptr_t vaddr`: Address where the MOBJ is mapped.
  - `BrMemFlags flags`: Access flags.

- Result: 
  - `BR_SUCCESS`: The mapping was created successfully.
  - `BR_BAD_HANDLE`: The handle of the `space` or `mobj` was invalid.
  - `BR_OUT_OF_MEMORY`: There was not enough memory to complete the operation.

This syscall finds some space in the target address space and maps the mobj into it.

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


### br_task

- Arguments:
  - (out) `BrTask task`: Handle of the task if created successfully (or `BR_MOBJ_ERROR`).
  - `BrSpace space`: Address space of the task.
  - `BrTaskArgs const* args`: Arguments of the task.
  - `BrTaskFlags flags`: Flags to tweaks the behavior of the task.

- Result: 
  - `BR_SUCCESS`: The task was created successfully.
  - `BR_BAD_HANDLE`: The handle to the space is invalid.
  - `BR_BAD_ADDRESS`: The address of the task was not located in user memory.

This syscall creates a new task.


### br_start

- Arguments:
  - `BrTask task`: Task to start.
  - `uintptr_t ip`: The instruction pointer of the task.
  - `uintptr_t sp`: The stack pointer of the task.
  - `BrTaskArgs* args`: The task argument.

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
  - `BR_SUCCESS`: The task was successfully exited.
  - `BR_BAD_HANDLE`: The task handle was invalid.

This syscall stops the execution of a task.

### br_send

- Result:
  - `BR_NOT_IMPLEMENTED`

Sends a message to another process.

### br_recv

- Result:
  - `BR_NOT_IMPLEMENTED`

Receives a message from another process.

### br_irq

- Result:
  - `BR_NOT_IMPLEMENTED`

Map an irq to a process.

### br_drop

- Arguments: 
  - `BrTask task_handle`: Target task 
  - `BrCap cap`: The capability to remove.

- Result:
  - `BR_SUCCESS`: The capability was successfully dropped.
  - `BR_BAD_HANDLE`: The task handle was invalid.

This syscall drop one or more task capabilities.

### br_close

- Arguments:
  - `BrHandle handle`: handle to close.

- Result: 
  - `BR_SUCCESS`: The handle was closed successfully.
  - `BR_BAD_HANDLE`: The handle was invalid.

This syscall closes a handle.
