/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * timers.hpp - Created on 15-08-2025
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

#include "psx/common/system/registers.h"

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

namespace PSX
{
    consteval int TimerMStoClock(int ms, int div)
    {
        return ((uint32_t)((((F_CPU) / (div)) * (ms)) / 1000));
    }

    // Common control flags shared by all timers
    enum CtrlFlag : uint16_t
    {
        CTRL_SYNC_ENABLE     = 1 << 0,
        CTRL_SYNC_BITMASK    = 3 << 1,
        CTRL_RESET_ON_TARGET = 1 << 3,
        CTRL_IRQ_ON_TARGET   = 1 << 4,
        CTRL_IRQ_ON_OVERFLOW = 1 << 5,
        CTRL_IRQ_REPEAT      = 1 << 6,
        CTRL_IRQ_TOGGLE      = 1 << 7,
        CTRL_EXT_CLOCK       = 1 << 8,
        CTRL_PRESCALE        = 1 << 9,
        CTRL_ACK_IRQ         = 1 << 10,
        CTRL_RELOADED        = 1 << 11,
        CTRL_OVERFLOWED      = 1 << 12
    };

    namespace Timer0
    {
        static volatile uint16_t &Value  = *_ADDR16(IO_BASE | 0x100);
        static volatile uint16_t &Ctrl   = *_ADDR16(IO_BASE | 0x104);
        static volatile uint16_t &Target = *_ADDR16(IO_BASE | 0x108);

        enum class SyncMode : uint16_t
        {
            PAUSE_ON_HBLANK       = 0 << 1,
            RESET_ON_HBLANK       = 1 << 1,
            RESET_ON_HBLANK_PAUSE = 2 << 1,
            WAIT_FOR_HBLANK       = 3 << 1
        };

        enum class ClockSource : uint16_t
        {
            SYSTEM = 0 << 8,
            DOTCLK = 1 << 8
        };
    } // namespace Timer0

    namespace Timer1
    {
        static volatile uint16_t &Value  = *_ADDR16(IO_BASE | 0x110);
        static volatile uint16_t &Ctrl   = *_ADDR16(IO_BASE | 0x114);
        static volatile uint16_t &Target = *_ADDR16(IO_BASE | 0x118);

        enum class SyncMode : uint16_t
        {
            PAUSE_ON_VBLANK       = 0 << 1,
            RESET_ON_VBLANK       = 1 << 1,
            RESET_ON_VBLANK_PAUSE = 2 << 1,
            WAIT_FOR_VBLANK       = 3 << 1
        };

        enum class ClockSource : uint16_t
        {
            SYSTEM = 0 << 8,
            HBLANK = 1 << 8
        };
    } // namespace Timer1

    namespace Timer2
    {
        constexpr int ClockFreq = (F_CPU / 8);

        static volatile uint16_t &Value  = *_ADDR16(IO_BASE | 0x120);
        static volatile uint16_t &Ctrl   = *_ADDR16(IO_BASE | 0x124);
        static volatile uint16_t &Target = *_ADDR16(IO_BASE | 0x128);

        enum class SyncMode : uint16_t
        {
            STOP = 0 << 1,
            RUN  = 1 << 1
        };

        enum class ClockSource : uint16_t
        {
            SYSTEM   = 0 << 8,
            SYS_DIV8 = 1 << 8
        };
    } // namespace Timer2

} // namespace PSX
