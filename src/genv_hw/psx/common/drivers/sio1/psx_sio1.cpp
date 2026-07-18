/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * SIO1_Bus.cpp - Created on 09-11-2025
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

#include <stdint.h>
#include <stddef.h>

#include "psx_sio1.hpp"
#include "psx/common/system/sys.h"

// TODO: SIO1

namespace PS1
{
    class PS1System;
}

namespace PS1::IO
{
    static constexpr int _SIO1_BAUD_RATE = 115200;

    int SIO1_Bus::init()
    {
        if (_initialised)
            return 0;

        SIO_CTRL(1)  = SIO_CTRL_RESET;
        SIO_MODE(1)  = SIO_MODE_BAUD_DIV1 | SIO_MODE_DATA_8;
        SIO_BAUD(1)  = F_CPU / _SIO1_BAUD_RATE;
        SIO_CTRL(1)  = SIO_CTRL_TX_ENABLE | SIO_CTRL_RX_ENABLE | SIO_CTRL_DSR_IRQ_ENABLE;
        _initialised = true;

        _initResult = 0;
        return _initResult;
    }

    bool SIO1_Bus::update()
    {
        return true;
    }

    void SIO1_Bus::m_sio1ISR()
    {
    }

} // namespace PS1::IO