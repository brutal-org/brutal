#include <host/time.h>
#include <time.h>

Tick host_time_current_tick(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)(ts.tv_nsec / 1000000) + ((uint64_t)ts.tv_sec * 1000ull);
}

TimeStamp host_time_current_timestamp(void)
{
    return (TimeStamp)time(NULL);
}
