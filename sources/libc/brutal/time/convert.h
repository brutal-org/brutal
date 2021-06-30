#pragma once

#include <brutal/time/types.h>

Date timestamp_to_date(TimeStamp timestamp);

Time timestamp_to_time(TimeStamp timestamp);

DateTime timestamp_to_datetime(TimeStamp timestamp);

TimeStamp DateTimeo_timestamp(DateTime datetime);
