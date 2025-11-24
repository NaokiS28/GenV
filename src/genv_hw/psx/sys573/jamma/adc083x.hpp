/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * adc083x.hpp - Created on 13-11-2025
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

#include <assert.h>
#include <stdint.h>

// #include "psx/core/registers.hpp"

namespace System::PSX
{
    class ADC038x
    {
    private:
        const uint8_t _numChannels;

    public:
        ADC038x(uint8_t channels) : _numChannels(channels)
        {
            // The *x* in ADC038x
            assert((channels == 1) || (channels == 2) ||
                   (channels == 4) || (channels == 8));
        }

        int getValue(uint8_t channel);
    };
} // namespace System::PSX