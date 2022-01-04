#pragma once

#include <brutal/parse/scan.h>

long scan_next_digit(Scan *self);

long scan_next_number(Scan *self);

long str_to_number(Str string);
