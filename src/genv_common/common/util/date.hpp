/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * date.hpp - Created on 15-08-2025
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

#pragma once

#include <stdint.h>
#include <stddef.h>
#include <time.h>

namespace Date
{
    enum Days : uint8_t
    {
        D_SUNDAY = 0,
        D_MONDAY,
        D_TUESDAY,
        D_WEDNESDAY,
        D_THURSDAY,
        D_FRIDAY,
        D_SATURDAY,
        D_INVALID
    };

    enum Months : uint8_t
    {
        M_JANUARY = 1,
        M_FEBURARY,
        M_MARCH,
        M_APRIL,
        M_MAY,
        M_JUNE,
        M_JULY,
        M_AUGUST,
        M_SEPTEMBER,
        M_OCTOBER,
        M_NOVEMBER,
        M_DECEMBER,
        M_INVALID
    };

    bool dateValid(int day, int month, int year);
    bool isLeapYear(int year);
    int getDayOfWeek(int day, int month, int year);
    int getMonthDayCount(int month, int year);
    int getYearDaysElapsed(tm &time);
    uint32_t toDOSTime(tm &time);
    size_t toString(char *output, tm &time);

    inline bool dateValid(tm time) { return dateValid(time.tm_mday, time.tm_mon, time.tm_year); }
    inline bool isLeapYear(tm time) { return isLeapYear(time.tm_year); }
    inline int getMonthDayCount(tm time) { return getMonthDayCount(time.tm_mon, time.tm_year); }
    inline int getDayOfWeek(tm time) { return getDayOfWeek(time.tm_mday, time.tm_mon, time.tm_year); }

};