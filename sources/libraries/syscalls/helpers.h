#pragma once

#include <syscalls/syscalls.h>

BrResult br_helper_close(BrHandle handle)
{
    return br_close(&(BrCloseArgs){
        .handle = handle,
    });
}
