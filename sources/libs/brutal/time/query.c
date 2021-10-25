#include <brutal/time/convert.h>
#include <brutal/time/query.h>
#include <embed/time.h>

Tick tick_now(void)
{
    return host_time_current_tick();
}

TimeStamp timestamp_now(void)
{
    return host_time_current_timestamp();
}

Date date_now(void)
{
    return datetime_now().date;
}

Time time_now(void)
{
    return datetime_now().time;
}

DateTime datetime_now(void)
{
    return timestamp_to_datetime(timestamp_now());
}
