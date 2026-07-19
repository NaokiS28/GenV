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
#include "common/services/io/iface_output.hpp"
#include "common/services/io/vjoy.hpp"
#include "common/services/services.hpp"
#include "common/util/templates.hpp"
#include "psx/common/system/sys.h"

#include "psx/gv999/io/io.hpp"

#define OUTPUT_PULSE_TIME 20

namespace SystemGV::IO
{
    using namespace Input;

    constexpr VJoy_Input player12ButtonMap[] = {
        VJoy_Input::D_Left,
        VJoy_Input::D_Right,
        VJoy_Input::D_Up,
        VJoy_Input::D_Down,
        VJoy_Input::Button_1,
        VJoy_Input::Button_2,
        VJoy_Input::Button_3,
        VJoy_Input::Button_4,
        VJoy_Input::Button_5,
        VJoy_Input::Start,
        INPUT_NONE_6};

    constexpr VJoy_Input player34ButtonMap[] = {
        VJoy_Input::D_Left,
        VJoy_Input::D_Right,
        VJoy_Input::D_Up,
        VJoy_Input::D_Down,
        VJoy_Input::Button_1,
        VJoy_Input::Button_2,
        VJoy_Input::Button_3,
        VJoy_Input::Start};

    constexpr VJoy_Arcade serviceButtonMap[] = {
        VJoy_Arcade::Coin_1,
        VJoy_Arcade::Coin_2,
        VJoy_Arcade::Service_1,
        VJoy_Arcade::Service_2,
        VJoy_Arcade::Test,
        ARCADE_NONE_3};

    int JAMMA::init()
    {
        int err = 0;
        err += playerManager()->attachInputDevice(&_devs[0], Input::Player::PLAYER_1);
        err += playerManager()->attachInputDevice(&_devs[1], Input::Player::PLAYER_2);
        err += playerManager()->attachInputDevice(&_devs[2], Input::Player::PLAYER_3);
        err += playerManager()->attachInputDevice(&_devs[3], Input::Player::PLAYER_4);
        err += playerManager()->attachInputDevice(&_devs[4], Input::Player::ARCADE_CABINET);
        return err;
    }

    bool JAMMA::update()
    {
        getJAMMAInputs();
        return GV_OK;
    }

    void JAMMA::getJAMMAInputs(void)
    {
        uint16_t inputs[3] = {0};

        // Read in raw data
        inputs[0] = IO::Regs::JammaP1;
        inputs[1] = IO::Regs::JammaP2;
        inputs[2] = IO::Regs::JammaP3P4;

        // P1/P2
        _digital[0] = getInputsFromMap((uint16_t)(~inputs[0] & (int)0xFFFF), player12ButtonMap);
        _digital[1] = getInputsFromMap((uint16_t)(~inputs[1] & (int)0xFFFF), player12ButtonMap);

        // P3/P4
        for (int i = 2; i < 4; i++) // Hoping this loop is unrolled by the compiler
        {
            // Convert main JAMMA port to GenV
            uint16_t temp = inputs[2] >> (8 * (i - 2));
            uint8_t x     = ~temp & (int)0xFF; // Inversion promotes to int
            _digital[i]   = getInputsFromMap(x, player34ButtonMap);
        }

        uint8_t service =
            (!(inputs[0] & JAMMA_P1P2_COIN) << 0) |
            (!(inputs[1] & JAMMA_P1P2_COIN) << 1) |
            (!(inputs[0] & JAMMA_P1P2_SERVICE) << 2) |
            (!(inputs[1] & JAMMA_P1P2_SERVICE) << 3) |
            (!(inputs[0] & JAMMA_P1_TEST) << 4);

        _digital[4] = getArcadeFromMap(service, serviceButtonMap); // System
    }
} // namespace SystemGV::IO
