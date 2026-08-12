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
#include "psx/sys573/io/asic.hpp"

namespace System573::IO
{
    void ADC038x::sendAddress(ADC038x_Channel ch, bool differential, bool sign)
    {
        if (numChannels == ADC038x_Channel::CH_0) return;

        uint8_t x = (static_cast<uint8_t>(ch) / 2);
        bool odd  = static_cast<uint8_t>(ch) % 2;

        exchangeBit(!differential);             // SIGNAL/!DIF
        exchangeBit(differential ? sign : odd); // ODD / SIGN

        if (numChannels == ADC038x_Channel::CH_7)
            exchangeBit(x & 0b10); // Select 1
        if (numChannels >= ADC038x_Channel::CH_3)
            exchangeBit(x & 0b01); // Select 0
    }

    bool ADC038x::exchangeBit(bool bit)
    {
        const ASIC::MiscOutput mask = ASIC::OUT_ADC_DI | ASIC::OUT_ADC_CLK;
        ASIC::MiscOutput x          = bit ? ASIC::OUT_ADC_DI : ASIC::OUT_NONE;

        // Set data
        ASIC::SetOutputsMasked(x, mask);

        // Clock high
        ASIC::SetOutputsMasked(x | ASIC::OUT_ADC_CLK, mask);
        auto miscin = ASIC::Regs::MiscIn;
        bool in     = (miscin & ASIC::IN_ADC_DO) != ASIC::IN_NONE;

        // Clock low
        x = (bit ? ASIC::OUT_ADC_DI : ASIC::OUT_NONE);
        ASIC::SetOutputsMasked(x, mask);
        return in;
    }

    uint8_t ADC038x::getValue(ADC038x_Channel ch, bool differential)
    {
        const auto max = channelToIndex(numChannels);
        auto c         = channelToIndex(ch);
        if (c >= max) c = max - 1;

        // CS active
        select(true);
        // Start bit
        exchangeBit(1);
        // MUX Select
        sendAddress(ch, differential);
        // ADC MUX settling period.
        exchangeBit(0);
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