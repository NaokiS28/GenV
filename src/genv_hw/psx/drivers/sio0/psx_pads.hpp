/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * psx_pads.hpp - Created on 15-11-2025
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

#include "common/services/services.hpp"
#include "common/services/io/inputman.hpp"
#include "common/util/hash.hpp"
#include "psx/psx_strings.hpp"
#include "psx_joy.hpp"

namespace System::PSX::IO
{
    class PSX_Controller : public Input::IInputDevice
    {
    private:
        PSX_Joypad *const _driver;
        const uint8_t _subportNumber;

    public:
        PSX_Controller(PSX_Joypad *driver, uint8_t port, InputDeviceType type, util::Hash id);
        ~PSX_Controller();

        inline virtual int poll() override { return _driver->poll(_subportNumber); }
    };

    class PSX_Digital : public PSX_Controller
    {
        inline PSX_Digital(PSX_Joypad *driver, uint8_t port, InputDeviceType type, util::Hash id)
            : PSX_Controller(driver, port, DEVICE_TYPE_CONTROLLER, "PS1 Digital"_h)
        {
            _name = PSX_DIGITAL_STR;
        }
    };
    class PSX_Analogue : public PSX_Controller
    {
        inline PSX_Analogue(PSX_Joypad *driver, uint8_t port, InputDeviceType type, util::Hash id)
            : PSX_Controller(driver, port, DEVICE_TYPE_CONTROLLER, "PS1 Analog"_h)
        {
            _name = PSX_ANALOG_STR;
        }
    };
    class PSX_DualShock : public PSX_Controller
    {
        inline PSX_DualShock(PSX_Joypad *driver, uint8_t port, InputDeviceType type, util::Hash id)
            : PSX_Controller(driver, port, DEVICE_TYPE_CONTROLLER, "PS1 DualShock"_h)
        {
            _name = PSX_DUALSHOCK_STR;
        }
    };
    class PSX_DualShock2 : public PSX_Controller
    {
        inline PSX_DualShock2(PSX_Joypad *driver, uint8_t port, InputDeviceType type, util::Hash id)
            : PSX_Controller(driver, port, DEVICE_TYPE_CONTROLLER, "PS1 DualShock2"_h)
        {
            _name = PSX_DUALSHOCK2_STR;
        }
    };

} // namespace System::PSX::IO