#include "convert.h"
#include "constants.h"

Date timestamp_to_date(TimeStamp timestamp)
{
    Date date = {};

    int days = timestamp / SECONDS_PER_DAY;

    date.year = EPOCH_YEAR;

    while (days - DAYS_PER_YEAR[is_leap_year(date.year)] > 0)
    {
        days -= DAYS_PER_YEAR[is_leap_year(date.year)];
        date.year++;
    }

    date.month = 0;

    while (days - DAYS_PER_MONTH[is_leap_year(date.year)][date.month] >= 0)
    {
        days -= DAYS_PER_MONTH[is_leap_year(date.year)][date.month];
        date.month++;
    }

    date.day = days + 1;
    date.month++;

    return date;
}

Time timestamp_to_time(TimeStamp timestamp)
{
    return (Time){
        .second = (int)(timestamp % 60),
        .minute = (int)((timestamp / SECONDS_PER_MINUTE) % 60),
        .hour = (int)((timestamp / SECONDS_PER_HOURS) % 24),
    };
}

DateTime timestamp_to_datetime(TimeStamp timestamp)
{
    return (DateTime){
        .time = timestamp_to_time(timestamp),
        .date = timestamp_to_date(timestamp),
    };
}

TimeStamp datetime_to_timestamp(DateTime datetime)
{
    TimeStamp timestamp = 0;

    for (Year year = EPOCH_YEAR; year < datetime.year; year++)
    {
        timestamp += DAYS_PER_YEAR[is_leap_year(year)] * SECONDS_PER_DAY;
    }

    for (Mounth month = 0; month < datetime.month - 1; month++)
    {
        timestamp += DAYS_PER_MONTH[is_leap_year(datetime.year)][month] * SECONDS_PER_DAY;
    }

    timestamp += (datetime.day - 1) * SECONDS_PER_DAY;
    timestamp += datetime.hour * SECONDS_PER_HOURS;
    timestamp += datetime.minute * SECONDS_PER_MINUTE;
    timestamp += datetime.second;

    return timestamp;
}
