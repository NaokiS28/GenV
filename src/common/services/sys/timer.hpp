/*
 * DXUX - Copyright (C) 2025 NaokiS, spicyjpeg
 * timer.hpp - Created on 24-05-2025
 *
 * DXUX is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * DXUX is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * DXUX. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once
#include <stdint.h>

typedef void (*TFunc)();

namespace System
{
    enum : uint8_t
    {
        TM_10MS = 1,
        TM_50MS = 5,
        TM_100MS = 10,
        TM_250MS = 25,
        TM_500MS = 50,
        TM_1SEC = 100
    };

    enum class TChannel
    {
        TIMER1,
        TIMER2,
        TIMER3,
        INVALID
    };

    constexpr const uint8_t FuncPerTimerChannel = 8;

    class Timer
    {
    private:
        struct RegisteredFunction
        {
            TFunc func = nullptr;
            uint16_t callFreq = 0;
            uint16_t lastCall = 0;

            RegisteredFunction(TFunc f, uint16_t freq) : func(f), callFreq(freq) {}
        };

        RegisteredFunction *rfList[FuncPerTimerChannel] = {nullptr};

    public:
        Timer() = default;
        virtual ~Timer();

        Timer(const Timer &) = delete;
        Timer &operator=(const Timer &) = delete;

        bool registerFunction(TFunc func, uint8_t freq);
        bool unregisterFunction(TFunc f);

        virtual void tick();
    };
} // namespace System
