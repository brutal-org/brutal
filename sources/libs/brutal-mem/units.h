#pragma once

#include <brutal-base>

#define KiB(VAL) ((uintptr_t)(VAL)*1024)

#define MiB(VAL) (KiB(VAL) * 1024)

#define GiB(VAL) (MiB(VAL) * 1024)

#define TiB(VAL) (GiB(VAL) * 1024)
