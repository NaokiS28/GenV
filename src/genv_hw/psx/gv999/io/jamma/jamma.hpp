/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * jamma.h - Created on 01-08-2025
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

#include <stdint.h>

#include "common/services/system/iface_driver.hpp"
#include "common/services/io/iface_input.hpp"
#include "common/services/system/iface_system.hpp"

namespace SystemGV::IO
{
    using namespace Input;

    enum JAMMAInput : uint32_t
    {
        // SYSGV_JAMMA_MAIN
        JAMMA_P1P2_LEFT    = 1 << 0,
        JAMMA_P1P2_RIGHT   = 1 << 1,
        JAMMA_P1P2_UP      = 1 << 2,
        JAMMA_P1P2_DOWN    = 1 << 3,
        JAMMA_P1P2_BUTTON1 = 1 << 4,
        JAMMA_P1P2_BUTTON2 = 1 << 5,
        JAMMA_P1P2_BUTTON3 = 1 << 6,
        JAMMA_P1P2_BUTTON4 = 1 << 7,
        JAMMA_P1P2_BUTTON5 = 1 << 8,
        JAMMA_P1P2_START   = 1 << 9,
        JAMMA_P1P2_COIN    = 1 << 10,
        JAMMA_P1P2_SERVICE = 1 << 11,
        JAMMA_P1_TEST      = 1 << 12,

        JAMMA_P3P4_LEFT    = 1 << 0,
        JAMMA_P3P4_RIGHT   = 1 << 1,
        JAMMA_P3P4_UP      = 1 << 2,
        JAMMA_P3P4_DOWN    = 1 << 3,
        JAMMA_P3P4_BUTTON1 = 1 << 4,
        JAMMA_P3P4_BUTTON2 = 1 << 5,
        JAMMA_P3P4_BUTTON3 = 1 << 6,
        JAMMA_P3P4_START   = 1 << 7,
        // JAMMA_P3P4_BUTTON4 = 1 << 7, // Not on P3/4?
        // JAMMA_P3P4_BUTTON5 = 1 << 8, // Not on P3/4?
    };

    class JAMMA : public System::IDriver
    {
    private:
        constexpr IInputDevice jamma(const char *name, uint32_t *digital)
        {
            return {
                name,
                "JAMMA"_h,
                0,
                Input::DEVICE_TYPE_CONTROLLER,
                0,
                10,
                digital,
                0,
                nullptr};
        };

        void getJAMMAInputs(void);

        uint32_t _digital[5]  = {0};
        IInputDevice _devs[5] = {
            _devs[0] = jamma("GV999 JAMMA (P1)", &_digital[0]),
            _devs[1] = jamma("GV999 JAMMA (P2)", &_digital[1]),
            _devs[2] = jamma("GV999 JAMMA (P3)", &_digital[2]),
            _devs[3] = jamma("GV999 JAMMA (P4)", &_digital[3]),
            _devs[4] = jamma("GV999 JAMMA Cabinet", &_digital[4]),
        };

    public:
        JAMMA(System::ISystem &system) : System::IDriver(system)
        {
            _name = "GV999 JAMMA";
        }

        int init() override;
        bool update() override;
    }; // namespace SystemGV::IO
} // namespace SystemGV::IO