#pragma once

#include "types.h"

Date timestamp_to_date(TimeStamp timestamp);

Time timestamp_to_time(TimeStamp timestamp);

DateTime timestamp_to_datetime(TimeStamp timestamp);

TimeStamp datetime_to_timestamp(DateTime datetime);
