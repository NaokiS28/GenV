/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * ifloat.cpp - Created on 21-06-2025
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

#include "ifloat.hpp"

ifloat intToiFloat(int value, int maxValue) {
    if (maxValue <= 0) {
        return 0.0f; // Fallback for invalid maxValue
    }

    ifloat result = static_cast<ifloat>(value) / static_cast<ifloat>(maxValue);

    // Clamp result to [0.0f, 1.0f]
    if (result < 0.0f) result = 0.0f;
    else if (result > 1.0f) result = 1.0f;

    return result;
}