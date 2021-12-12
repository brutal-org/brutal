#include "kernel/cpu.h"

void embed_enter_critical_section(void) { cpu_retain_interrupts(); }

void embed_leave_critical_section(void) { cpu_release_interrupts(); }
