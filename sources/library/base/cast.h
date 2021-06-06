#pragma once

#define base_cast(instance) (&(instance)->base)
#define cast_from_uint32(type, ptr) ((type)(uintptr_t)(ptr))
