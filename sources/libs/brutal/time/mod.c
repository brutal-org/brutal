#include <brutal/io/emit.h>
#include <brutal/time/mod.h>
#include <embed/time.h>

static bool time_is_leap_year(int year)
{
    return year % 4 == 0 && (year % 100 != 0 || year % 400 == 0);
}

static int time_days_per_year(int year)
{
    return time_is_leap_year(year) ? 366 : 365;
}

static int time_days_per_month(int year, int month)
{
    static int const DAYS_PER_MONTH[2][12] = {
        {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
        {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
    };

    return DAYS_PER_MONTH[time_is_leap_year(year)][month];
}

static Time time_per_year(int year)
{
    return time_days_per_year(year) * TIME_DAY;
}

static Time time_per_month(int year, int month)
{
    return time_days_per_month(year, month) * TIME_DAY;
}

Time time_now(void)
{
    return embed_time_now();
}

Date time_to_date(Time time)
{
    uint64_t secs = time / TIME_SEC;
    int64_t days = secs / TIME_DAY;

    Date date = {};

    date.years = TIME_EPOCH;

    while (days >= time_days_per_year(date.years))
    {
        days -= time_days_per_year(date.years);
        date.years++;
    }

    date.months = 0;

    while (days >= time_days_per_month(date.years, date.months))
    {
        days -= time_days_per_month(date.years, date.months);
        date.months++;
    }

    date.days = days + 1;
    date.months++;

    return date;
}

static bool time_emit(Scan *scan, Emit *emit, Date date)
{
    if (scan_skip_word(scan, str$("YYYYY")))
    {
        emit_fmt$(emit, "{04d}", date.years);
        return true;
    }
    else if (scan_skip_word(scan, str$("MM")))
    {
        emit_fmt$(emit, "{02d}", date.months);
        return true;
    }
    else if (scan_skip_word(scan, str$("DD")))
    {
        
    }

    return false;
}

IoResult time_fmt(IoWriter writer, Str fmt, Time time)
{
    size_t written = 0;

    Scan scan;
    scan_init(&scan, fmt);

    return OK(IoResult, written);
}

IoResult time_iso(IoWriter writer, Time time)
{
    return time_fmt(writer, str$("YYYY-MM-DDTHH:mm:ss.sssZ"), time);
}

Date date_now(void)
{
    return time_to_date(time_now());
}

Time date_to_time(Date date)
{
    Time time = 0;

    for (uint64_t year = TIME_EPOCH; year < date.years; year++)
    {
        time += time_secs_per_year(year);
    }

    for (uint64_t month = 0; month < date.months - 1; month++)
    {
        time += time_secs_per_month(date.years, month);
    }

    time += (date.days - 1) * TIME_DAY;
    time += date.hours * TIME_HOUR;
    time += date.minutes * TIME_MIN;
    time += date.seconds * TIME_SEC;

    return time;
}
