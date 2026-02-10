/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * jvs.cpp - Created on 26-11-2025
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

#include "jvs.hpp"
#include "common/return_codes.hpp"
#include "common/services/io/iface_input.hpp"

namespace System::PSX
{

    constexpr IInputDevice jvs(uint32_t *digital, int16_t *analog = nullptr)
    {
        return {
            GX700_JVS_NAME,
            "SYS573JVS"_h,
            0,
            Input::DEVICE_TYPE_CONTROLLER,
            0,
            0,
            digital,
            0,
            analog};
    };

    int Sys573JVS::init()
    {
        return GV_OK;
    }

    int Sys573JVS::update()
    {
        return GV_OK;
    }

    void Sys573JVS::processPackets_()
    {
        if (_packetBuffer.available())
        {
        }
    }

} // namespace System::PSX
