#pragma once

#include <brutal/base/std.h>

typedef uint64_t br_tick_t;
typedef uint64_t br_timestamp_t;

typedef uint64_t br_seconds_t;
typedef uint64_t br_minutes_t;
typedef uint64_t br_hours_t;

typedef struct
{
    br_seconds_t seconds;
    br_minutes_t minutes;
    br_hours_t hours;
} br_time_t;

typedef uint64_t br_day_t;
typedef uint64_t br_month_t;
typedef uint64_t br_year_t;

typedef struct
{
    br_day_t day;
    br_month_t month;
    br_year_t year;
} br_date_t;

typedef union
{
    struct
    {
        br_day_t day;
        br_month_t month;
        br_year_t year;

        br_seconds_t secondes;
        br_minutes_t minutes;
        br_hours_t hours;
    };

    struct
    {
        br_date_t date;
        br_time_t time;
    };
} br_datetime_t;
