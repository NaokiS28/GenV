/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * jamma.cpp - Created on 23-08-2025
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

#include "jamma.hpp"
#include "common/return_codes.hpp"
#include "common/services/io/iface_input.hpp"
#include "common/services/services.hpp"

namespace System::PSX
{

    constexpr IInputDevice jamma(PlayerSuggestion player, uint32_t *digital, int16_t *analog = nullptr)
    {
        return {
            GX700_JAMMA_NAME,
            "SYS573JAMMA"_h,
            0,
            Input::DEVICE_TYPE_CONTROLLER,
            Input::DEVICE_SUBTYPE_STANDARD,
            0,
            player,
            {10,
             (player == Input::DEVICE_PLAYER_1 ? GX700_ANALOG_COUNT : static_cast<uint8_t>(0)),
             0,
             0},
            {digital,
             analog}};
    };

    int Sys573Jamma::init()
    {
        _devs[0] = jamma(Input::DEVICE_PLAYER_1, &_digital[0], _analog);
        _devs[1] = jamma(Input::DEVICE_PLAYER_2, &_digital[1]);
        _devs[2] = jamma(Input::DEVICE_ARCADE_CABINET, &_digital[2]);
        int err  = 0;
        for (auto &dev : _devs)
            err += Services::attachInputDevice(&dev);
        return err;
    }

    int Sys573Jamma::update()
    {
        getJAMMAInputs();
        return GV_OK;
    }

    void Sys573Jamma::getJAMMAInputs(void)
    {
        uint32_t inputs[3];

        // Read in raw data
        inputs[0] = SYS573_JAMMA_MAIN;
        inputs[1] = (SYS573_JAMMA_EXT1 & 0x0f00) >> 8;
        inputs[2] = (SYS573_JAMMA_EXT2 & 0x0f00) >> 8;

        // Remap Konami's mapping to GenV
        uint32_t lr = (inputs[0] & 0x00030003) << 2;
        uint32_t ud = (inputs[0] & 0x000C000C) >> 2;
        uint32_t st = (inputs[0] & 0x80008000) >> 3;
        uint32_t bt = (inputs[0] & 0x70007000) << 1;
        inputs[0]   = (ud | lr | st | bt);

        _digital[0] = ((inputs[0] & 0xFF00) >> 8) | (inputs[1] & 0x0B);    // Player 1
        _digital[1] = (inputs[0] & 0x00FF) | (inputs[2] & 0x0B);           // Player 2
        _digital[2] = (SYS573_MISC_IN & 0x1300) >> 8 | (inputs[1] & 0x04); // System
    }

    void Sys573Jamma::getAnalogInputs()
    {
        int c = 0;
        for (auto &ch : _analog)
            ch = _adc.getValue(c++);
    }
} // namespace System::PSX
