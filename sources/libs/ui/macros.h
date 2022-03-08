#pragma once

#include <ui/funcs.h>

#define ui_use_state$(UI, STATE) (typeof(STATE) *)ui_use_state(UI, &(STATE), sizeof(STATE))
#define ui_scope$(UI, TYPE, ...) defer$(TYPE##_begin(UI __VA_OPT__(, ) __VA_ARGS__), ui_end(UI))
#define ui_begin$(UI) ({if (!ui_begin(UI)) return false; true; })
#define ui_main$(FUN) \
    int main() { return 0; }
