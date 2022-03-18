#pragma once

#include <brutal/base/attributes.h>
#include <brutal/base/std.h>
#include <brutal/io/fmt.h>
#include <brutal/text/str.h>

#define TIME_EPOCH (1970)
#define END_OF_TIME ((Time)-1)

typedef uint64_t Time;

#define TIME_NS ((Time)1)
#define TIME_US ((Time)1000 * TIME_NS)
#define TIME_MS ((Time)1000 * TIME_US)
#define TIME_SEC ((Time)1000 * TIME_MS)
#define TIME_MIN ((Time)60 * TIME_SEC)
#define TIME_HOUR ((Time)60 * TIME_MIN)
#define TIME_DAY ((Time)24 * TIME_HOUR)

#define time_us$(V) (Time)(V)
#define time_ns$(V) ((Time)(V)*TIME_NS)
#define time_ms$(V) ((Time)(V)*TIME_MS)
#define time_sec$(V) ((Time)(V)*TIME_SEC)
#define time_min$(V) ((Time)(V)*TIME_MIN)
#define time_hour$(V) ((Time)(V)*TIME_HOUR)
#define time_day$(V) ((Time)(V)*TIME_DAY)

typedef union
{
    uint64_t seconds;
    uint64_t minutes;
    uint64_t hours;
    uint64_t days;
    uint64_t months;
    uint64_t years;
} Date;

Time time_now(void);

Date time_to_date(Time time);

IoResult time_fmt(IoWriter writer, Str fmt, Time time);

IoResult time_iso(IoWriter writer, Time time);

#define time_fmt$(FMT, TIME) time_fmt(str$(FMT), TIME)

Date date_now(void);

Time date_to_time(Date date);
