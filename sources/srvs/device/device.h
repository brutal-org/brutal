#pragma once

#include <brutal-ds>

typedef struct _Device Device;

struct _Device
{
    Vec(Device *) devices;
};
