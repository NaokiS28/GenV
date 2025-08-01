/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * sys573.cpp - Created on 01-08-2025
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

#include "sys573.hpp"

namespace System
{
    bool Sys573System::init()
    {
    }

    int Sys573System::update()
    {
    }

    bool Sys573System::shutdown()
    {
    }

    uint8_t Sys573System::increaseCoinCounter(uint8_t counter)
    {
        if (counter >= physicalCoinSlots)
            return 0xFF;

        if (counter < 2)
        {
            // Shift over the counter (so it's either CC1 or CC2), set, wait, unset
            SYS573_MISC_OUT |= (SYS573_MISC_OUT_COIN_COUNT1 << counter);
            for (int i = 100; i > 0; i--)
                ;
            SYS573_MISC_OUT ^= (SYS573_MISC_OUT_COIN_COUNT1 << counter);
            return counter;
        }
        else
        {
            return ArcadeSystem::increaseCoinCounter(counter);
        }
    }

    uint8_t Sys573System::setOutputs(uint8_t bank, uint8_t data)
    {
        if(bank >= outputBanks)
            return 0xff;
        
        if(bank == 0){
            SYS573_EXT_OUT = data;
            return 0;
        } else {
            // JVS/IO
            ArcadeSystem::setOutputs(bank, data);
        }
    }
    uint8_t Sys573System::setSingleOutput(uint8_t outputNumber, bool state)
    {
        if(outputNumber >= (8 * outputBanks))
            return 0xff;
        
        if(outputNumber < 8){
            if(state)
                SYS573_EXT_OUT |= (uint8_t)(state << outputNumber);
            else 
                SYS573_EXT_OUT &= ~(uint8_t)(state << outputNumber);
            return outputNumber;
        } else {
            // JVS/IO
            ArcadeSystem::setSingleOutput(outputNumber, state);
        }
    }
}