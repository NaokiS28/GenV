/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * soft_rtc.hpp - Created on 16-08-2025
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

#include <time.h>
#include "iface_rtc.hpp"

namespace Time
{
    class SoftRTC : public IRTC
    {
    protected:
        tm clock;
        size_t ticks = 0;

    public:
        SoftRTC();
        virtual ~SoftRTC() = default;

        // Clock settings
        virtual void tick();
        virtual int setTime(int hour, int min, int sec, bool amPm);
        virtual int setDate(int day, int month, int year);
        virtual int setTimezone(int offset);
        virtual int setDST(bool useDST);
        virtual int getTime(tm &time)
        {
            time = clock;
            return true;
        }

        // Get the current time
        virtual size_t getUnixTime();
        virtual int getLocalTime(tm *tmObj, time_t time);
    };
} // namespace Time