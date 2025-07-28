/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * timer.cpp - Created on 24-05-2025
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

#include "timer.hpp"
#include "common/util/misc.hpp"

using namespace System;

Timer::~Timer()
{
    for (auto &t : rfList)
    {
        if (t != nullptr)
        {
            delete t;
            t = nullptr;
        }
    }
}

void Timer::tick()
{
    for (auto &t : rfList)
    {
        if (t)
        {
            t->lastCall++;
            if (t->lastCall >= t->callFreq)
            {
                t->func();
                t->lastCall = 0;
            }
        }
    }
}

bool Timer::registerFunction(TFunc f, uint8_t freq)
{
    if (!f)
        return false;

    for (auto &t : rfList)
    {
        if (!t)
        {
            t = new RegisteredFunction(f, freq);
            return true;
        }
    }

    // No empty slots available
    return false;
}

bool Timer::unregisterFunction(TFunc f)
{
    if (!f)
        return false;

    for (auto &t : rfList)
    {
        if (t->func == f)
        {
            delete t;
            t = nullptr;
            return true;
        }
    }

    // Function was not found
    return false;
}