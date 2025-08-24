/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * time.hpp - Created on 16-08-2025
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

namespace Time
{
    bool timeValid(int hour, int min, int sec);
    void addSecond(tm &time);
    void addDay(tm &time);
    int getTimeString(tm &time, char *str, bool seconds = true, bool amPm = false);
    
    inline bool timeValid(tm time){ return timeValid(time.tm_hour, time.tm_min, time.tm_sec); }
}