#pragma once

#include <brutal-base>

typedef uint64_t Tick;
typedef uint64_t TimeStamp;

#define TIME_TIMEOUT_INFINITY ((Timeout)-1)
typedef uint64_t Timeout;
typedef uint64_t Deadline;

typedef uint64_t NanoSeconds;
typedef uint64_t Seconds;
typedef uint64_t Minutes;
typedef uint64_t Hours;

typedef struct PACKED
{
    Seconds second;
    Minutes minute;
    Hours hour;
} Time;

typedef uint64_t Day;
typedef uint64_t Mounth;
typedef uint64_t Year;

typedef struct PACKED
{
    Day day;
    Mounth month;
    Year year;
} Date;

typedef union PACKED
{
    struct
    {
        Day day;
        Mounth month;
        Year year;

        Seconds second;
        Minutes minute;
        Hours hour;
    };

    struct
    {
        Date date;
        Time time;
    };
} DateTime;
