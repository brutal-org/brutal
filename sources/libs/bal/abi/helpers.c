#include <bal/abi/helpers.h>
#include <brutal-debug/log.h>

char const *br_syscall_to_string(BrSyscall syscall)
{
    static char const *SYSCALL_NAMES[] = {
#define ITER(SYSCALL) #SYSCALL,
        FOREACH_SYSCALLS(ITER)
#undef ITER
    };

    if (syscall >= BR_SYSCALL_COUNT)
    {
        return "INVALID";
    }

    return SYSCALL_NAMES[syscall];
}

char const *br_result_to_string(BrResult result)
{
    static char const *RESULT_NAMES[] = {
#define ITER(RESULT) #RESULT,
        FOREACH_RESULTS(ITER)
#undef ITER
    };

    if (result >= BR_RESULT_COUNT)
    {
        return "INVALID";
    }

    return RESULT_NAMES[result];
}

Error br_result_to_error(BrResult result)
{
    switch (result)
    {
    case BR_SUCCESS:
        return ERR_SUCCESS;

    case BR_BAD_ADDRESS:
        return ERR_BAD_ADDRESS;

    case BR_OUT_OF_MEMORY:
        return ERR_OUT_OF_MEMORY;

    case BR_BAD_ARGUMENTS:
        return ERR_BAD_ARGUMENTS;

    case BR_NOT_PERMITTED:
        return ERR_DENIED;

    case BR_BAD_HANDLE:
        return ERR_BAD_HANDLE;

    case BR_NOT_IMPLEMENTED:
        return ERR_NOT_IMPLEMENTED;

    case BR_BAD_SYSCALL:
        return ERR_BAD_SYSCALL;

    case BR_TIMEOUT:
        return ERR_TIMEOUT;

    case BR_WOULD_BLOCK:
        return ERR_WOULD_BLOCK;

    default:
        return ERR_UNDEFINED;
    }
}

bool br_event_eq(BrEvent a, BrEvent b)
{
    if (a.type != b.type)
    {
        return false;
    }

    switch (a.type)
    {
    case BR_EVENT_IRQ:
        return a.irq == b.irq;

    default:
        panic$("Unhandled event type: {}", a.type);
    }
}

bool br_addr_eq(BrAddr a, BrAddr b)
{
    return a.id == b.id && (a.port == b.port || a.port == BR_PORT_ANY || b.port == BR_PORT_ANY);
}
