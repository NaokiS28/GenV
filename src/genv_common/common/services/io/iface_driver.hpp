/*
 * GenV - Copyright (C) 2025 - 2026 NaokiS, spicyjpeg
 * iface_driver.hpp - Created on 23-02-2026
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

#include "common/return_codes.hpp"
#include "common/util/hash.hpp"

namespace IO
{
    class PlayerManager; // forward declaration

    // Base class for all IO drivers. A driver represents a hardware bus or input source
    // (e.g. JAMMA, JVS, Windows HID). During init(), drivers attach their devices to
    // PlayerManager. During update(), drivers refresh device state buffers.
    class IDriver
    {
    protected:
        const char *_name = nullptr;
        util::Hash id;

    public:
        IDriver() = default;
        IDriver(util::Hash id) : id(id) {}
        virtual ~IDriver() = default;
        virtual int init(PlayerManager &playerManager) { return GV_OK; };
        virtual bool update() = 0;
        virtual bool reset() { return GV_OK; };
        virtual void shutdown() {};

        const char *getName() { return _name; }
    };
} // namespace IO