#pragma once

#include <brutal/parse/scan.h>

long scan_next_digit(Scan *self);

bool scan_next_uint(Scan *self, unsigned long *value);

bool scan_next_int(Scan *self, long *value);

bool scan_next_float(Scan *self, float *value);

bool str_to_uint(Str string, unsigned long *value);

bool str_to_int(Str string, long *value);

bool str_to_float(Str string, float *value);
