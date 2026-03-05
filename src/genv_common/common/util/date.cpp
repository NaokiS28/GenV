/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * date.cpp - Created on 15-08-2025
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

#include "date.hpp"

#include "time.hpp"

#include <stdio.h>
#include <time.h>

#include "common/services/services.hpp"

namespace Date
{

    bool dateValid(int day, int month, int year)
    {
        if (month > 12)
            return false;
        if ((day < 1) || (day > getMonthDayCount(month, year)))
            return false;

        return true;
    }

    bool isLeapYear(int year)
    {
        if (year % 4)
            return false;
        if (!(year % 100) && (year % 400))
            return false;

        return true;
    }

    int getDayOfWeek(int day, int month, int year)
    {
        // See https://datatracker.ietf.org/doc/html/rfc3339#appendix-B
        int _year = year, _month = month - 2;

        if (_month <= 0)
        {
            _month += 12;
            _year--;
        }

        int century = _year / 100;
        _year %= 100;

        int weekday = 0 + day + (_month * 26 - 2) / 10 + _year + _year / 4 + century / 4 + century * 5;

        return weekday % 7;
    }

    int getMonthDayCount(int month, int year)
    {
        switch (month)
        {
        case 2:
            return isLeapYear(year) ? 29 : 28;

        case 4:
        case 6:
        case 9:
        case 11:
            return 30;

        default:
            return 31;
        }
    }

    int getYearDaysElapsed(tm &time)
    {
        int days = 0;
        for (int m = 0; m < time.tm_mon; m++)
            days += getMonthDayCount(m, time.tm_year);
        days += (time.tm_mday - 1);
        return days;
    }

    uint32_t toDOSTime(tm &time)
    {
        int _year = time.tm_year - 80; // tm_year is sans 1900

        if (!dateValid(time) || !Time::timeValid(time))
            return 0;
        if ((_year < 0) || (_year > 127))
            return 0;

        return 0 | (_year << 25) | (time.tm_mon << 21) | (time.tm_mday << 16) | (time.tm_hour << 11) | (time.tm_min << 5) | (time.tm_sec >> 1);
    }

    size_t toString(char *output, tm &time)
    {
        if (!dateValid(time) || !Time::timeValid(time))
        {
            *output = 0;
            return 0;
        }

        return snprintf(
            output, 32, "%04d-%02d-%02d %02d:%02d:%02d",
            time.tm_year, time.tm_mon, time.tm_mday,
            time.tm_hour, time.tm_min, time.tm_sec);
    }

} // namespace Date