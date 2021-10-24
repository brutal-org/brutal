#include <bal/abi.h>
#include <bal/helpers.h>
#include <brutal/alloc.h>
#include <brutal/debug.h>
#include <brutal/io.h>
#include <brutal/time.h>

int main(int argc, char const *argv[])
{

    log$("Hello from the posix server!");

    DateTime dt = timestamp_to_datetime(brh_global_info()->time);
    log$("Time is {}/{}/{}", dt.day, dt.month, dt.year);

    log$("ARGC={}", argc);
    log$("ARGV={#x}", (uintptr_t)argv);

    for (int i = 0; i < argc; i++)
    {
        log$("ARGV[{}]={}", i, argv[i]);
    }

    return 0;
}
