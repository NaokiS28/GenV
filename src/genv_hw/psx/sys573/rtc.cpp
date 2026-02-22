/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * 573_rtc.cpp - Created on 16-08-2025
 *
 * GenV is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * GenV is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * GenV. If not, see <https://www.gnu.org/licenses/>.
 */

#include "rtc.hpp"
#include "common/logger/log.hpp" // IWYU pragma: keep
#include "common/services/system/rtc/soft_rtc.hpp"
#include "registers573.hpp"
#include "common/util/misc.hpp"
#include "common/util/time.hpp"
#include "common/util/date.hpp"

namespace System573
{

    constexpr auto rtcNvram = "NVRAM";

    int RTC::init()
    {
        getClock(clock);
        SYS573_RTC_DAY |= SYS573_RTC_DAY_BATTERY_MONITOR;
        RTC_is_ok = !(SYS573_RTC_DAY & SYS573_RTC_DAY_LOW_BATTERY);
        if (!RTC_is_ok) LOG_RTC(Time::rtcBattLowFmt);
        return RTC_is_ok;
    }

    int RTC::setTime(int hour, int min, int sec, bool amPm)
    {
        if (!Time::timeValid(hour, min, sec))
            return 1;
        SYS573_RTC_CTRL |= SYS573_RTC_CTRL_WRITE;
        SYS573_RTC_HOUR   = util::dec2bcd(hour);
        SYS573_RTC_MINUTE = util::dec2bcd(min);
        SYS573_RTC_SECOND = util::dec2bcd(sec) & (SYS573_RTC_SECOND_TENS_BITMASK | SYS573_RTC_SECOND_UNITS_BITMASK);
        SYS573_RTC_CTRL &= ~(SYS573_RTC_CTRL_WRITE);

        if (!RTC_is_ok) LOG_RTC(Time::rtcSetWithBadBattFmt, Time::rtcTimeString, Time::rtcTimeString);

        return 0;
    }

    int RTC::setDate(int day, int month, int year)
    {
        if (!Date::dateValid(day, month, year))
            return 1;
        SYS573_RTC_CTRL |= SYS573_RTC_CTRL_WRITE;

        bool century       = year > 1999;
        SYS573_RTC_WEEKDAY = (Date::getDayOfWeek(year, month, day) & (century << 4));

        SYS573_RTC_YEAR  = util::dec2bcd(year);
        SYS573_RTC_MONTH = util::dec2bcd(month);
        SYS573_RTC_DAY   = (util::dec2bcd(day) & (SYS573_RTC_DAY_TENS_BITMASK | SYS573_RTC_DAY_UNITS_BITMASK));
        SYS573_RTC_CTRL &= ~(SYS573_RTC_CTRL_WRITE);

        if (!RTC_is_ok) LOG_RTC(Time::rtcSetWithBadBattFmt, Time::rtcDateString, Time::rtcDateString);

        return 0;
    }

    void RTC::tick()
    {
        SoftRTC::tick();
        if (RTC_is_ok && ticks > RTCSyncTime)
        {
            // Sync with the physical RTC every so often if the battery is good
            // If not, then the RTC features will be disabled to prevent time/date corruption
            getTime(clock);
            ticks = 0;
        }
    }

    int RTC::getTime(tm &time)
    {
        if (!RTC_is_ok) return SoftRTC::getTime(time);

        SYS573_RTC_CTRL |= SYS573_RTC_CTRL_READ;
        time.tm_hour = util::bcd2dec(SYS573_RTC_HOUR);
        time.tm_min  = util::bcd2dec(SYS573_RTC_MINUTE);
        time.tm_sec  = util::bcd2dec(SYS573_RTC_SECOND & (SYS573_RTC_SECOND_TENS_BITMASK | SYS573_RTC_SECOND_UNITS_BITMASK));
        SYS573_RTC_CTRL &= ~(SYS573_RTC_CTRL_READ);
        return 0;
    }

    int RTC::getDate(tm &time)
    {
        if (!RTC_is_ok) return SoftRTC::getDate(time);

        SYS573_RTC_CTRL |= SYS573_RTC_CTRL_READ;
        time.tm_mday = util::bcd2dec(SYS573_RTC_DAY & (SYS573_RTC_DAY_TENS_BITMASK | SYS573_RTC_DAY_UNITS_BITMASK));
        time.tm_mon  = util::bcd2dec(SYS573_RTC_MONTH);
        time.tm_year = util::bcd2dec(SYS573_RTC_YEAR);
        SYS573_RTC_CTRL &= ~(SYS573_RTC_CTRL_READ);
        return 0;
    }

    size_t RTC::getUnixTime()
    {
        return 0;
    }

    int RTC::getLocalTime(tm *tmObj, time_t time)
    {
        return 0;
    }

    int RTC::readNVRAM(uint8_t *data, int offset, int count)
    {
        if (!data || count >= SYS573_RTC_SIZE)
            return 0;

        offset %= SYS573_RTC_SIZE; // Allow mirroring

        for (int c = 0; c < count; c++)
            data[c] = SYS573_RTC_SRAM[offset + c];

        return count;
    }

    int RTC::writeNVRAM(const uint8_t *data, int offset, int count)
    {
        if (!data || count >= SYS573_RTC_SIZE || offset >= SYS573_RTC_SIZE)
            return 0;

        for (int c = 0; c < count; c++)
            SYS573_RTC_SRAM[offset + c] = data[c];

        if (!RTC_is_ok) LOG_RTC(Time::rtcSetWithBadBattFmt, rtcNvram, rtcNvram);

        return count;
    };
} // namespace System573