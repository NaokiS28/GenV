/*
 * GenV - Copyright (C) 2025 - 2026 NaokiS, spicyjpeg
 * ide_bus.cpp - Created on 24-07-2026
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

#include "ide_bus.hpp"
#include "common/return_codes.hpp"

namespace IDE
{
    int BaseIDEDriver::init()
    {
        return GV_OK;
    }

    bool BaseIDEDriver::reset()
    {
        return true;
    }

    bool BaseIDEDriver::update()
    {
        return true;
    }

    void BaseIDEDriver::shutdown()
    {
    }

    void BaseIDEDriver::isr()
    {
        // IDE Device asserted IRQ
    }

} // namespace IDE