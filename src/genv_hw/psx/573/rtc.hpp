/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * 573_rtc.hpp - Created on 16-08-2025
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

#include <string.h>

#include "registers573.hpp"
#include "common/services/system/rtc/soft_rtc.hpp"
#include "common/services/system/arcade/nvram.hpp"

namespace System::PSX::KSYS573
{
    constexpr const int RTCSyncTime = (60 * 15); // Seconds x Minutes
    class RTC : public Time::SoftRTC
    {
    private:
        bool RTC_is_ok = false;

    public:
        RTC();
        ~RTC() override {};

        void tick() override;
        int setTime(int hour, int min, int sec, bool amPm) override;
        int setDate(int day, int month, int year) override;
        tm getTime(){ return clock; }
        bool getTime(tm &time);
        int getDate(tm &time);
        int getClock(tm &time) { return (getTime(time) | getDate(time)); }

        bool batteryStatus(){ return RTC_is_ok; }

        size_t getUnixTime();
        int getLocalTime(tm *tmObj, time_t time);

        int readNVRAM(uint8_t *data, int offset, int count);
        int writeNVRAM(const uint8_t *data, int offset, int size);

    };
}