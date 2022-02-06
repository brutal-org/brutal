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
