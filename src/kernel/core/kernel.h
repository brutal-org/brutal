#pragma once

#define KERNEL_STACK_SIZE (16384)
#define USER_STACK_BASE (0xc0000000)
#define IPC_CHANNEL_SIZE (256)

void kernel_splash(void);
