#include "kernel/cpu.h"

void embed_sync_enter(void) { cpu_retain_interrupts(); }

void embed_sync_leave(void) { cpu_release_interrupts(); }
