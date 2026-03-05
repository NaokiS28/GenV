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

#include "psx/common/registers.hpp"
#include "common/services/system/rtc/iface_rtc.hpp"
#include "common/services/system/rtc/soft_rtc.hpp"

namespace System573::RTC
{
    typedef enum
    {
        CTRL_CAL_BITMASK  = 31 << 0,
        CTRL_CAL_POSITIVE = 0 << 5,
        CTRL_CAL_NEGATIVE = 1 << 5,
        CTRL_READ         = 1 << 6,
        CTRL_WRITE        = 1 << 7
    } ControlFlag;

    typedef enum
    {
        SECOND_UNITS_BITMASK = 15 << 0,
        SECOND_TENS_BITMASK  = 7 << 4,
        SECOND_STOP          = 1 << 7
    } SecondFlag;

    typedef enum
    {
        WEEKDAY_UNITS_BITMASK  = 7 << 0,
        WEEKDAY_CENTURY        = 1 << 4,
        WEEKDAY_CENTURY_ENABLE = 1 << 5,
        WEEKDAY_FREQUENCY_TEST = 1 << 6
    } WeekdayFlag;

    typedef enum
    {
        DAY_UNITS_BITMASK   = 15 << 0,
        DAY_TENS_BITMASK    = 3 << 4,
        DAY_LOW_BATTERY     = 1 << 6,
        DAY_BATTERY_MONITOR = 1 << 7
    } DayFlag;

    static constexpr uint32_t BaseAddr      = DEV0_BASE | 0x620000;
    static constexpr uint32_t ClockBaseAddr = _A8_to_A16(BaseAddr, 0x1FF8);
    static constexpr int      SRAMSize      = (8 * 1024) - 0x000F;

    namespace Regs
    {
        static volatile uint16_t &Ctrl    = *_ADDR16(_A8_to_A16(ClockBaseAddr, 0x00));
        static volatile uint16_t &Second  = *_ADDR16(_A8_to_A16(ClockBaseAddr, 0x01));
        static volatile uint16_t &Minute  = *_ADDR16(_A8_to_A16(ClockBaseAddr, 0x02));
        static volatile uint16_t &Hour    = *_ADDR16(_A8_to_A16(ClockBaseAddr, 0x03));
        static volatile uint16_t &Weekday = *_ADDR16(_A8_to_A16(ClockBaseAddr, 0x04));
        static volatile uint16_t &Day     = *_ADDR16(_A8_to_A16(ClockBaseAddr, 0x05));
        static volatile uint16_t &Month   = *_ADDR16(_A8_to_A16(ClockBaseAddr, 0x06));
        static volatile uint16_t &Year    = *_ADDR16(_A8_to_A16(ClockBaseAddr, 0x07));
    } // namespace Regs

    struct NVRAMView
    {
        volatile uint16_t *const ptr = _ADDR16(BaseAddr);

        volatile uint16_t &operator[](int offset) const
        {
            return ptr[offset % SRAMSize];
        }
    };

    static const NVRAMView SRAM;

    // We sync every 15 minutes since we don't need precision
    constexpr const int SyncTime = (60 * 15); // Seconds x Minutes

    class M48T58 : public Time::SoftRTC
    {
    private:
        bool rtc_is_ok = false;

    public:
        int init() override;

        void tick() override;
        int setTime(int hour, int min, int sec, bool amPm) override;
        int setDate(int day, int month, int year) override;
        tm getTime() { return clock; }
        int getTime(tm &time) override;
        int getDate(tm &time) override;
        int getClock(tm &time) { return (getTime(time) | getDate(time)); }

        uint8_t getBatteryState() override { return (rtc_is_ok ? Time::GV_RTC_GOOD : Time::GV_RTC_BATTERY_DEAD); }

        size_t getUnixTime() override;
        int getLocalTime(tm *tmObj, time_t time) override;

        int readNVRAM(uint8_t *data, int offset, int count);
        int writeNVRAM(const uint8_t *data, int offset, int size);
    };
} // namespace System573::RTC