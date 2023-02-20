#pragma once

#include <brutal/base/str.h>

typedef struct
{
    int interval;
    Str *frames;
} CliSpinner;

Str cli_spinner(CliSpinner spinner);

extern CliSpinner const CLI_SPINNER_DOTS;
