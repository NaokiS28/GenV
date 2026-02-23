/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * inputman.hpp - Created on 22-08-2025
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

#include "common/util/templates.hpp"
#include "iface_driver.hpp"
#include "playermgr.hpp"
#include "common/services/adminkey.hpp"
#include <string.h>

namespace IO
{
    static constexpr const int initialDrivers = 16;

    class DeviceManager
    {
    private:
        util::PointerList<IDriver *, initialDrivers> _driverList;
        PlayerManager &_playerManager;

    public:
        inline DeviceManager(AdminClass_Key key, PlayerManager &playerManager)
            : _playerManager(playerManager) { assert(_driverList.ready()); }
        ~DeviceManager();

        int init();
        void update();
        void reset();
        void shutdown();

        int registerDriver(IDriver *device);
        int unregisterDriver(IDriver *device);

        inline const char *name(size_t idx)
        {
            if (idx >= _driverList.length()) return nullptr;
            return _driverList.at(idx)->getName();
        }

        inline size_t count() { return _driverList.length(); }
    };
} // namespace IO