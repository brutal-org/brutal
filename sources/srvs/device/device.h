#pragma once

#include <brutal/ds.h>

typedef struct _Device Device;

struct _Device
{
    Vec(Device *) devices;
};
