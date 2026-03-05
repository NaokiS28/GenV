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
#include "common/util/enum_defs.hpp"
#include "psx/sys573/io/asic.hpp"
#include "psx/sys573/registers573.hpp"
#include <assert.h>
#include <stdint.h>

// #include "psx/psx/registers.hpp"

// TODO: Does not support differential mode. But is this worth supporting?
// TODO: Critical: Implementation does not work. Require logic probing.

namespace System573::IO
{
    enum class ADC038x_Channel : uint8_t
    {
        CH_0    = 0,
        CH_1    = 1,
        CH_2    = 2,
        CH_3    = 3,
        CH_4    = 4,
        CH_5    = 5,
        CH_6    = 6,
        CH_7    = 7,
        INVALID = 0xFF
    };

    ENABLE_BITWISE_OPS(ADC038x_Channel);

    constexpr ADC038x_Channel channelTotalToADC(uint8_t channels)
    {
        switch (channels)
        {
        case 1: return ADC038x_Channel::CH_0;
        case 2: return ADC038x_Channel::CH_1;
        case 4: return ADC038x_Channel::CH_3;
        case 8: return ADC038x_Channel::CH_7;
        default: return ADC038x_Channel::INVALID;
        }
    }

    constexpr ADC038x_Channel channelIndexToADC(uint8_t channel)
    {
        switch (channel)
        {
        case 0: return ADC038x_Channel::CH_0;
        case 1: return ADC038x_Channel::CH_1;
        case 2: return ADC038x_Channel::CH_2;
        case 3: return ADC038x_Channel::CH_3;
        case 4: return ADC038x_Channel::CH_4;
        case 5: return ADC038x_Channel::CH_5;
        case 6: return ADC038x_Channel::CH_6;
        case 7: return ADC038x_Channel::CH_7;
        default: return ADC038x_Channel::INVALID;
        }
    }

    class ADC038x
    {
    private:
        const ADC038x_Channel numChannels;

        bool exchangeBit(bool bit);

        void sendAddress(ADC038x_Channel ch, bool differential = false, bool sign = false);

        inline void select(bool state)
        {
            ASIC::SetOutputBit(ASIC::OUT_ADC_CS, !state);
        }

    public:
        ADC038x(ADC038x_Channel channels) : numChannels(channels)
        {
            // The *x* in ADC038x
            assert((channels == ADC038x_Channel::CH_0) ||
                   (channels == ADC038x_Channel::CH_1) ||
                   (channels == ADC038x_Channel::CH_3) ||
                   (channels == ADC038x_Channel::CH_7));
        }

        static constexpr uint8_t channelToIndex(ADC038x_Channel channel)
        {
            return static_cast<uint8_t>(channel);
        }

        static constexpr ADC038x_Channel indexToChannel(uint8_t channel)
        {
            return static_cast<ADC038x_Channel>(channel > 7 ? 7 : channel);
        }

        uint8_t getValue(ADC038x_Channel ch, bool differential = false);
    };
} // namespace System573::IO