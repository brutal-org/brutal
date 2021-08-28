#include <brutal/alloc.h>
#include <brutal/io.h>
#include <brutal/log.h>
#include <brutal/time.h>
#include <syscalls/helpers.h>
#include <syscalls/syscalls.h>

int br_entry()
{
    log$("Hello from the posix server!");

    DateTime dt = timestamp_to_datetime(brh_global_info()->time);
    log$("Time is {}/{}/{}", dt.day, dt.month, dt.year);

    return 0;
}
