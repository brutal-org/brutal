#pragma once

#include "scan.h"

long scan_next_digit(Scan *self);

bool scan_next_uint(Scan *self, unsigned long *value);

bool scan_next_int(Scan *self, long *value);

#ifndef __freestanding__

bool scan_next_float(Scan *self, float *value);

bool scan_next_double(Scan *self, double *value);

#endif

bool str_to_uint(Str string, unsigned long *value);

bool str_to_int(Str string, long *value);

#ifndef __freestanding__

bool str_to_float(Str string, double *value);

#endif
