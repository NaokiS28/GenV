/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * psx_pads.cpp - Created on 15-11-2025
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

#include "psx_pads.hpp"

namespace System::PSX::IO
{
    PSX_Controller::PSX_Controller(PSX_Joypad *driver, uint8_t port, InputDeviceType type, util::Hash id)
        : Input::IInputDevice(type, id), _driver(driver), _subportNumber(port)
    {
        Services::attachInputDevice(this);
    }

    PSX_Controller::~PSX_Controller()
    {
        Services::dettachInputDevice(this);
    }
} // namespace System::PSX::IO