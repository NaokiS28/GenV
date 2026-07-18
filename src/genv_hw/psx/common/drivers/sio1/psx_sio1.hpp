/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * psx_sio0.hpp - Created on 09-11-2025
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
#include <stddef.h>

#include "common/services/system/iface_driver.hpp"
#include "psx/common/psx_strings.hpp"
#include "terminal/terminal.h"

namespace PS1::IO
{
    enum SIO1State : uint8_t
    {
        SIO1_OKAY,
        SIO1_IN_USE,
        SIO1_NO_RESPONSE,
    };

    class SIO1_Bus : public ::System::IDriver
    {
        friend class PS1_BaseSystem;

    private:
        bool _initialised = false;
        int _initResult   = 0;
        bool _inUse       = false;

        void m_sio1ISR();

    public:
        inline SIO1_Bus(::System::ISystem &sys) : ::System::IDriver(sys)
        {
            _name = PS1_PS_SIO1_STR;
        }
        int init() override;
        bool update() override;
    };
} // namespace PS1::IO