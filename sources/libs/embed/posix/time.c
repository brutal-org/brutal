#include <embed/time.h>
#include <time.h>
#include <brutal-base>

Tick embed_time_current_tick(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)(ts.tv_nsec / 1000000) + ((uint64_t)ts.tv_sec * 1000ull);
}

unsigned long embed_time_current_nsec(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_nsec) + ((uint64_t)ts.tv_sec * 1000000000ull);
}

TimeStamp embed_time_current_timestamp(void)
{
    return (TimeStamp)time(nullptr);
}
