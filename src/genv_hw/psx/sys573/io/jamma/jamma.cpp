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
#include "common/services/io/vjoy.hpp"
#include "common/services/services.hpp"
#include "common/util/templates.hpp"
#include "psx/common/system/sys.h"
#include "psx/sys573/io/jamma/adc083x.hpp"
#include "psx/sys573/registers573.hpp"

namespace System573::IO
{
    using namespace Input;
    constexpr IInputDevice jamma(const char *name, uint32_t *digital, uint8_t numAnalog = 0, int16_t *analog = nullptr)
    {
        return {
            name,
            "JAMMA"_h,
            0,
            Input::DEVICE_TYPE_CONTROLLER,
            0,
            10,
            digital,
            numAnalog,
            analog};
    };

    constexpr VJoy_Input playerButtonMap[] = {
        VJoy_Input::D_Left,
        VJoy_Input::D_Right,
        VJoy_Input::D_Up,
        VJoy_Input::D_Down,
        VJoy_Input::Button_1,
        VJoy_Input::Button_2,
        VJoy_Input::Button_3,
        VJoy_Input::Start};

    constexpr VJoy_Input extraButtonMap[] = {
        VJoy_Input::Button_4,
        VJoy_Input::Button_5,
        VJoy_Input::None,
        VJoy_Input::Button_6,
        INPUT_NONE_4};

    constexpr VJoy_Arcade serviceButtonMap[] = {
        VJoy_Arcade::Coin_1,
        VJoy_Arcade::Coin_2,
        VJoy_Arcade::Test,
        ARCADE_NONE_1,
        VJoy_Arcade::Service_1,
        ARCADE_NONE_3};

    int JAMMA::init()
    {
        _devs[0] = jamma("System 573 JAMMA (P1)", &_digital[0], GX700_ANALOG_COUNT, _analog);
        _devs[1] = jamma("System 573 JAMMA (P2)", &_digital[1]);
        _devs[2] = jamma("System 573 JAMMA (Service)", &_digital[2]);

        int err = 0;
        err += getServiceManager()->attachInputDevice(&_devs[0], Input::Player::PLAYER_1);
        err += getServiceManager()->attachInputDevice(&_devs[1], Input::Player::PLAYER_2);
        err += getServiceManager()->attachInputDevice(&_devs[2], Input::Player::ARCADE_CABINET);
        ASIC::writeOutputBit(MiscOutput::ADC_CS, true);
        return err;
    }

    bool JAMMA::update()
    {
        getJAMMAInputs();
        getAnalogInputs();
        return GV_OK;
    }

    void JAMMA::getJAMMAInputs(void)
    {
        uint16_t inputs[3] = {0};

        // Read in raw data
        inputs[0] = SYS573_JAMMA_MAIN;
        inputs[1] = (SYS573_JAMMA_EXT1 & 0x0f00) >> 8;
        inputs[2] = (SYS573_JAMMA_EXT2 & 0x0f00) >> 8;

        for (int i = 0; i < 2; i++) // Hoping this loop is unrolled by the compiler
        {
            // Convert main JAMMA port to GenV
            uint16_t temp = inputs[0] >> (8 * (1 - i));
            uint8_t x     = ~temp & (int)0xFF; // Inversion promotes to int
            _digital[i]   = getInputsFromMap(x, playerButtonMap);

            // Then the extra buttons on a second port
            x = (~(inputs[1 + i])) & 0x0F; // Inversion promotes to int
            _digital[i] |= getInputsFromMap(x, extraButtonMap);
        }

        uint8_t service = (~SYS573_MISC_IN >> 8) & 0x13;
        service |= ~inputs[1] & 0x04;
        _digital[2] = getArcadeFromMap(service, serviceButtonMap); // System
    }

    void JAMMA::getAnalogInputs()
    {
        // Update over the course of x frames to avoid adding to much burden.
        static int c = 0;
        auto &ch     = _analog[c];

        // Debug marker for signiture in logic analyser
        if (!c)
        {
            ASIC::writeOutputBit(MiscOutput::ADC_CS, false);
            psx_delayMicrosecondsBusy(10);
            ASIC::writeOutputBit(MiscOutput::ADC_CS, true);
        }

        ch = util::map(
            _adc.getValue(ADC038x::indexToChannel(c)),
            0, UINT8_MAX, -1024, 1023);
        c++;
        if (c >= GX700_ANALOG_COUNT) c = 0;
    }
} // namespace System573::IO
