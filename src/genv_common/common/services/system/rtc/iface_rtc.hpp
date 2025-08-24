/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * iface_rtc.hpp - Created on 15-08-2025
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

#include "common/util/time.hpp"

namespace Time
{
    class IRTC
    {
    public:
        IRTC() = default;
        virtual ~IRTC() = default;

        // Clock settings
        virtual void tick() = 0;
        virtual int setTime(int hour, int min, int sec, bool amPm) = 0;
        virtual int setDate(int day, int month, int year) = 0;
        virtual int setTimezone(int offset) = 0;
        virtual int setDST(bool useDST) = 0;
        virtual bool getTime(tm &time) = 0;

        // Get the current time
        virtual size_t getUnixTime() = 0;
        virtual int getLocalTime(tm *tmObj, time_t time) = 0;
    };
}