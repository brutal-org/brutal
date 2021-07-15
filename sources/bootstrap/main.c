#include "syscalls/syscalls.h"

int main(int argc, char const *argv[]);

void _start()
{
    main(0, (char const **)0);
}

int main(MAYBE_UNUSED int argc, MAYBE_UNUSED char const *argv[])
{
    br_log("Hello, world!", 13);

    while (1)
    {
    }

    return 0;
}
