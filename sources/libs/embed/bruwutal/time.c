#include <bal/abi.h>
#include <embed/time.h>

Tick embed_time_current_tick(void)
{
    BrNowArgs now;
    br_now(&now);
    return now.tick;
}

TimeStamp embed_time_current_timestamp(void)
{
    BrNowArgs now;
    br_now(&now);
    return now.timestamp;
}

unsigned long embed_time_current_nsec(void)
{
    return embed_time_current_tick() * 1000000000ull;
}
