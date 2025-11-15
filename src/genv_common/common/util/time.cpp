/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * time_util.cpp - Created on 16-08-2025
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

#include "time.hpp"
#include "date.hpp"

#include <stdio.h>
#include <time.h>

namespace Time
{
    bool timeValid(int hour, int min, int sec)
    {
        if ((hour > 23) || (min > 59) || (sec > 59))
            return false;
        return true;
    }

    void addSecond(tm &time)
    {
        time.tm_sec++;
        if (time.tm_sec >= 60)
        {
            time.tm_min++;
            time.tm_sec = 0;
            if (time.tm_min >= 60)
            {
                time.tm_hour++;
                time.tm_min = 0;
                if (time.tm_hour >= 24)
                {
                    time.tm_hour = 0;
                    addDay(time);
                }
            }
        }
    }

    void addDay(tm &time)
    {
        time.tm_mday++;
        time.tm_yday++;
        int monthDays = Date::getMonthDayCount(time);
        if (time.tm_mon == Date::M_FEBURARY && Date::isLeapYear(time))
            monthDays++;

        if (time.tm_yday >= 365)
        {
            time.tm_mday = 1;
            time.tm_mon  = 0; // C tm struct is 0-based months.
            time.tm_year++;
            time.tm_yday = 0;
        }

        if (time.tm_mday > monthDays)
        {
            time.tm_mday = 1;
            time.tm_mon++;
            if (time.tm_mon > 11)
            {
                time.tm_mon = 0; // C tm struct is 0-based months.
                time.tm_year++;
                time.tm_yday = 0;
            }
        }
    }

    int getTimeString(tm &time, char *str, size_t len, bool seconds, bool amPm)
    {
        if (!str || (seconds && len < 12) || (!seconds && len < 8))
            return 1;

        int hour;
        bool pm = false;
        if (time.tm_hour > 12)
        {
            pm   = true;
            hour = time.tm_hour - 12;
        }
        else
        {
            hour = time.tm_hour;
        }

        if (!seconds)
            snprintf(
                str, 8, "%02u%c%02u%s",
                hour, (time.tm_sec % 2 ? ' ' : ':'), time.tm_min,
                (amPm ? (pm ? "PM" : "AM") : ""));
        else
            snprintf(
                str, 12, "%02u:%02u:%02u%s",
                hour, time.tm_min, time.tm_sec,
                (amPm ? (pm ? " PM" : " AM") : ""));
        return 0;
    }
} // namespace Time