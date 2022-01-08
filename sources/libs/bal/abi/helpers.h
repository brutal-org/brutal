#pragma once

#include <bal/abi/types.h>

char const *br_syscall_to_string(BrSyscall syscall);

char const *br_result_to_string(BrResult result);

Error br_result_to_error(BrResult result);

bool br_event_eq(BrEvent a, BrEvent b);

bool br_addr_eq(BrAddr a, BrAddr b);
