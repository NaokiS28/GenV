/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * soft_rtc.cpp - Created on 16-08-2025
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

#include "soft_rtc.hpp"
#include "common/util/time.hpp"
#include "common/util/date.hpp"

namespace Time
{
    SoftRTC::SoftRTC()
    {
        clock.tm_sec = 0;
        clock.tm_min = 0;
        clock.tm_hour = 0;
        clock.tm_mday = 1;
        clock.tm_mon = 0;
        clock.tm_year = (2025 - 1990);
        clock.tm_yday = 0;
        clock.tm_isdst = false;
    }

    void SoftRTC::tick()
    {
        ticks++;
        Time::addSecond(clock);
    }

    int SoftRTC::setTime(int hour, int min, int sec, bool amPm)
    {
        if(!Time::timeValid(hour, min, sec))
            return 1;
        clock.tm_hour = hour;
        clock.tm_min = min;
        clock.tm_sec = sec;
        return 0;
    }

    int SoftRTC::setDate(int day, int month, int year)
    {
        if(!Date::dateValid(day, month, year))
            return 1;
        clock.tm_mday = day;
        clock.tm_mon = month;
        clock.tm_year = year;
        clock.tm_yday = Date::getYearDaysElapsed(clock);
        return 0;
    }

    int SoftRTC::setTimezone(int offset)
    {
        return 0;
    }

    int SoftRTC::setDST(bool useDST)
    {
        return 0;
    }

    size_t SoftRTC::getUnixTime()
    {
        return 0;
    }

    int SoftRTC::getLocalTime(tm *tmObj, time_t time)
    {
        return 0;
    }

} // namespace Time
