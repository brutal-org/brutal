#pragma once

#include <bal/boot/handover.h>
#include <brutal/ds.h>
#include "driver.h"

typedef struct {
    void *ctx;
    NicDriver *driver;
} Interface;

typedef Vec(Interface) VecInterface;

extern VecInterface interfaces;

void interface_init(Handover *handover);
