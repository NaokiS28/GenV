/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * arcade.hpp - Created on 01-08-2025
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
#include "nvram.hpp"
#include "errorcodes.hpp"

/*
 * GenV - Arcade Extenstions
 * Systems which are arcade platforms (or can work as arcade platforms natively) should
 * be derrived from this class which allows arcade game titles to access the physical
 * devices of a system.
 *
 * It is expected that the minimum of the following features are supported:
 * * Coin Inputs - Must support a minimum of 1 coin/credit device
 * * Coin Counter - Must support a minimum of 1 counter PER coin/credit device
 * * Player Inputs - This must be a minimum of 1 player with 2 directions and 3 buttons.
 * * Service Inputs - Must support a minimum of a Test button/switch and a Service credit input
 * * NVRAM Device support - This can be an battery backed RTC device, flash or other non-volatile data storage device.
 * * Watchdog - If the hardware has a watchdog, it MUST be supported by Genv.
 */

namespace System
{
    namespace ArcadeEnv
    {
        struct DIPSwitches
        {
            uint8_t banks = 0;
            uint8_t *array = nullptr;
        };

        struct CoinData
        {
            uint8_t coinsIn = 0;
            uint8_t counterTicks = 0;
        };
    } // namespace ArcadeEnv
} // namespace System