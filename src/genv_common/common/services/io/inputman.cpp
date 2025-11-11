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
#include "common/services/io/iface_input.hpp"
#include <string.h>

#define ILOG(fmt, ...) LOG("inputman", fmt __VA_OPT__(, ) __VA_ARGS__)

namespace Input
{
    const char *szIMFailed = "%s device failed: %s";
    const char *szDeviceNull = "device pointer is null.";
    const char *szAttach = "Attach";
    const char *szDetach = "Detach";

    constexpr const int startingListSize = 4;

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
        deviceList = new IInput *[startingListSize];
        if (!deviceList)
            return 1;
        deviceListLength = startingListSize;
        for (int i = 0; i < startingListSize; i++)
        {
            deviceList[i] = nullptr;
        }
        return 0;
    }

    void InputManager::update()
    {
        for (int i = 0; i < deviceCount; i++)
        {
            deviceList[i]->update();
        }
    }

    void InputManager::reset()
    {
        for (int i = 0; i < deviceCount; i++)
        {
            deviceList[i]->reset();
        }
    }

    void InputManager::shutdown()
    {
        for (int i = 0; i < deviceCount; i++)
        {
            deviceList[i]->shutdown();
        }
    }

    bool InputManager::attachDevice(Input::IInput *dev)
    {
        if (!dev)
        {
            ILOG(szIMFailed, szAttach, szDeviceNull);
            return false;
        }
        if (deviceCount == deviceListLength)
        {
            auto tempArr = new IInput *[deviceCount + 1];
            if (!tempArr)
                return false;

            memcpy(tempArr, deviceList, sizeof(IInput *) * (deviceCount + 1));
            delete[] deviceList;
            deviceList = tempArr;
            deviceListLength++;
        }
        deviceList[deviceCount] = dev;
        deviceCount++;
        return true;
    }

    bool InputManager::detachDevice(Input::IInput *dev)
    {
        if (!dev)
        {
            ILOG(szIMFailed, szDetach, szDeviceNull);
            return false;
        }
        for (int i = 0; i < deviceCount; i++)
        {
            if (deviceList[i] == dev)
            {
                auto dev = deviceList[i];
                dev->shutdown();
                delete dev;
                memmove(&deviceList[i], &deviceList[i + 1], sizeof(IInput *) * (deviceCount - i));
            }
        }
        return true;
    }
} // namespace Input