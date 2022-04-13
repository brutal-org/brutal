#pragma once 
#include <kernel/hyper.h>
#include <kernel/x86_64/vt-x/vt-x.h>

void hypervisor_enable(void);

void hypervisor_disable(void);

void hypervisor_enter(void*);

