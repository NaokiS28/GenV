/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * inputman.cpp - Created on 22-08-2025
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

#include "inputman.hpp"
#include "common/logger/log.hpp"

#define ILOG(fmt, ...) LOG("inputman", fmt __VA_OPT__(, ) __VA_ARGS__)

namespace Input
{
    const char *szIMFailed = "%s device failed: %s";
    const char *szDeviceNull = "device pointer is null.";
    const char *szAttach = "Attach";
    const char *szDetach = "Detach";

    InputManager::InputManager(AdminClass_Key key)
    {
    }

    InputManager::~InputManager()
    {
        if (deviceList)
            delete[] deviceList;
    }

    int InputManager::init()
    {
        return 0;
    }

    void InputManager::update()
    {
    }

    void InputManager::shutdown()
    {
    }

    bool InputManager::attachDevice(Input::IInput *dev)
    {
        if (!dev)
        {
            ILOG(szIMFailed, szAttach, szDeviceNull);
            return false;
        }
        return true;
    }

    bool InputManager::detachDevice(Input::IInput *dev)
    {
        if (!dev)
        {
            ILOG(szIMFailed, szDetach, szDeviceNull);
            return false;
        }
        return true;
    }
} // namespace Input