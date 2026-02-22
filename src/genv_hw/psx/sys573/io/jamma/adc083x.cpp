/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * adc083x.cpp - Created on 13-11-2025
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

#include "adc083x.hpp"
#include "psx/common/system/sys.h"
#include "psx/sys573/io/asic.hpp"
#include "psx/sys573/registers573.hpp"

namespace System573::IO
{
    bool ADC038x::exchangeBit(bool bit)
    {
        const MiscOutput mask = static_cast<MiscOutput>(MiscOutput::ADC_DI | MiscOutput::ADC_CLK);
        MiscOutput x          = bit ? MiscOutput::ADC_DI : MiscOutput::NONE;

        // Set data
        ASIC::writeOutputsMasked(x, mask);

        // Clock high
        ASIC::writeOutputsMasked(x | MiscOutput::ADC_CLK, mask);
        auto miscin = static_cast<MiscInput>(SYS573_MISC_IN);
        bool in     = (miscin & MiscInput::ADC_DO) != MiscInput::NONE;

        // Clock low
        psx_delayMicrosecondsBusy(1);
        x = (bit ? MiscOutput::ADC_DI : MiscOutput::NONE);
        ASIC::writeOutputsMasked(x, mask);
        return in;
    }

    uint8_t ADC038x::getValue(ADC038x_Channel ch, bool singleChannel)
    {
        const auto max = channelToIndex(numChannels);
        auto c         = channelToIndex(ch);
        if (c >= max) c = max - 1;

        // CS active
        select(true);
        // Start bit
        exchangeBit(1);
        if (numChannels > ADC038x_Channel::CH_1)
            exchangeBit(singleChannel);

        for (uint8_t i = max - 1; i > 0; i >>= 1)
            exchangeBit(c & i);

        exchangeBit(0); // ADC MUX settling period.

        // We no longer care about transmitting (DI is disabled)
        int8_t result = 0;
        for (uint8_t i = 0; i < 17; i++)
        {
            // We only want MSB first and only care about the first 8 bits
            // We only transmit 17-bits just to follow the datasheet

            // If we were caring for SARS
            /*
            if (numChannels >= ADC038x_Channel::CH_4)
            {
                if (SYS573_MISC_IN & SYS573_MISC_IN_ADC_SARS)
                    result = (result << 1) | exchangeBit(0);
            }
            else */
            // But as we are not.
            bool thisBit = exchangeBit(0);
            if (i < 8) result = (result << 1) | thisBit;

            // ADC0831 exits early.
            if (numChannels == ADC038x_Channel::CH_1 && i >= 11) break;
        }
        select(false);
        return result;
    }

} // namespace System573::IO