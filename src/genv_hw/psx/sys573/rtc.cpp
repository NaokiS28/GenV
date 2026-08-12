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
#include "common/util/misc.hpp"
#include "common/util/time.hpp"
#include "common/util/date.hpp"

namespace System573::RTC
{

    constexpr auto rtcNvram = "NVRAM";

    int M48T58::init()
    {
        getClock(clock);
        Regs::Day |= DAY_BATTERY_MONITOR;
        rtc_is_ok = !(Regs::Day & DAY_LOW_BATTERY);
        if (!rtc_is_ok) LOG_RTC(Time::rtcBattLowFmt);
        return rtc_is_ok;
    }

    int M48T58::setTime(int hour, int min, int sec, bool amPm)
    {
        if (!Time::timeValid(hour, min, sec))
            return 1;
        Regs::Ctrl |= CTRL_WRITE;
        Regs::Hour   = util::dec2bcd(hour);
        Regs::Minute = util::dec2bcd(min);
        Regs::Second = util::dec2bcd(sec) & (SECOND_TENS_BITMASK | SECOND_UNITS_BITMASK);
        Regs::Ctrl &= ~(CTRL_WRITE);

        if (!rtc_is_ok) LOG_RTC(Time::rtcSetWithBadBattFmt, Time::rtcTimeString, Time::rtcTimeString);

        return 0;
    }

    int M48T58::setDate(int day, int month, int year)
    {
        if (!Date::dateValid(day, month, year))
            return 1;
        Regs::Ctrl |= CTRL_WRITE;

        bool century  = year > 1999;
        Regs::Weekday = (Date::getDayOfWeek(year, month, day) & (century << 4));
        Regs::Year    = util::dec2bcd(year);
        Regs::Month   = util::dec2bcd(month);
        Regs::Day     = (util::dec2bcd(day) & (DAY_TENS_BITMASK | DAY_UNITS_BITMASK));
        Regs::Ctrl &= ~(CTRL_WRITE);

        if (!rtc_is_ok) LOG_RTC(Time::rtcSetWithBadBattFmt, Time::rtcDateString, Time::rtcDateString);

        return 0;
    }

    void M48T58::tick()
    {
        SoftRTC::tick();
        if (rtc_is_ok && ticks > SyncTime)
        {
            // Sync with the physical RTC every so often if the battery is good
            // If not, then the RTC features will be disabled to prevent time/date corruption
            getTime(clock);
            ticks = 0;
        }
    }

    int M48T58::getTime(tm &time)
    {
        if (!rtc_is_ok) return SoftRTC::getTime(time);

        Regs::Ctrl |= CTRL_READ;
        time.tm_hour = util::bcd2dec(Regs::Hour);
        time.tm_min  = util::bcd2dec(Regs::Minute);
        time.tm_sec  = util::bcd2dec(Regs::Second & (SECOND_TENS_BITMASK | SECOND_UNITS_BITMASK));
        Regs::Ctrl &= ~(CTRL_READ);
        return 0;
    }

    int M48T58::getDate(tm &time)
    {
        if (!rtc_is_ok) return SoftRTC::getDate(time);

        Regs::Ctrl |= CTRL_READ;
        time.tm_mday = util::bcd2dec(Regs::Day & (DAY_TENS_BITMASK | DAY_UNITS_BITMASK));
        time.tm_mon  = util::bcd2dec(Regs::Month);
        time.tm_year = util::bcd2dec(Regs::Year);
        Regs::Ctrl &= ~(CTRL_READ);
        return 0;
    }

    size_t M48T58::getUnixTime()
    {
        return 0;
    }

    int M48T58::getLocalTime(tm *tmObj, time_t time)
    {
        return 0;
    }

    int M48T58::readNVRAM(uint8_t *data, int offset, int count)
    {
        if (!data || count >= SRAMSize)
            return 0;

        for (int c = 0; c < count; c++)
            data[c] = SRAM[offset + c];

        return count;
    }

    int M48T58::writeNVRAM(const uint8_t *data, int offset, int count)
    {
        if (!data || count >= SRAMSize)
            return 0;

        for (int c = 0; c < count; c++)
            SRAM[offset + c] = data[c];

        if (!rtc_is_ok) LOG_RTC(Time::rtcSetWithBadBattFmt, rtcNvram, rtcNvram);

        return count;
    };
} // namespace System573::RTC