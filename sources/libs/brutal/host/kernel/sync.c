#include "kernel/cpu.h"

void host_enter_critical_section(void) { cpu_retain_interrupts(); }

void host_leave_critical_section(void) { cpu_release_interrupts(); }
