#pragma once 
#include <brutal/base.h>

typedef struct
{
    int v;
} HypervisorConfig;

bool hypervisor_is_hw_acceleration_available(void);

void hypervisor_enable(void);

void hypervisor_disable(void);

void hypervisor_enter(void*);
