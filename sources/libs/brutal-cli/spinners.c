#include <brutal-debug>
#include <brutal-time>

#include "spinners.h"

Str cli_spinner(CliSpinner spinner)
{
    Tick now = tick_now();
    uint64_t frame = now / spinner.interval;

    int n = 0;
    while (!str_eq(spinner.frames[n], nullstr))
    {
        n++;
    }

    return spinner.frames[frame % n];
}

Str CLI_SPINNER_DOTS_FRAMES[] = {
    str_const$("⠋"),
    str_const$("⠙"),
    str_const$("⠹"),
    str_const$("⠸"),
    str_const$("⠼"),
    str_const$("⠴"),
    str_const$("⠦"),
    str_const$("⠧"),
    str_const$("⠇"),
    str_const$("⠏"),
    nullstr,
};

CliSpinner const CLI_SPINNER_DOTS = {
    .interval = 80,
    .frames = CLI_SPINNER_DOTS_FRAMES,
};
