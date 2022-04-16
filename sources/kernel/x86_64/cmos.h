#pragma once

#include <brutal-time>

#define CMOS_ADDRESS 0x70
#define CMOS_DATA 0x71

enum cmos_regs
{
    CMOS_RTC_SECOND = 0x00,
    CMOS_RTC_MINUTE = 0x02,
    CMOS_RTC_HOUR = 0x04,
    CMOS_RTC_DAY = 0x07,
    CMOS_RTC_MONTH = 0x08,
    CMOS_RTC_YEAR = 0x09,

    CMOS_STATUS_A = 0x0A,
};

uint8_t cmos_read(enum cmos_regs reg);

DateTime cmos_read_rtc(void);
