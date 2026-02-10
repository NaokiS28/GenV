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
    const char szInputFailedFmt[] = "%s %s failed to %s: %s";
    const char szInputFmt[] = "%s %s %sed.";
    const char szAttach[] = "attach";
    const char szDetach[] = "detach";
    const char szRegister[] = "register";
    const char szUnregister[] = "unregister";
    const char szInput[] = "Input";
    const char szDriver[] = "driver";
    const char szDevice[] = "device";

    const char szDeviceNull[] = "device pointer is null.";

    InputManager::~InputManager()
    {
        for (auto driver : _driverList)
            if (driver) delete driver;
    }

    int InputManager::init()
    {
        if (!_driverList.ready() || !_devList.ready())
            return 1;
        return 0;
    }

    void InputManager::update()
    {
        for (auto driver : _driverList)
            if (driver) driver->update();

        // for (auto pad : _devList)
    }

    void InputManager::reset()
    {
        for (auto driver : _driverList)
            if (driver) driver->reset();
    }

    void InputManager::shutdown()
    {
        for (auto driver : _driverList)
            if (driver) driver->shutdown();
    }

    int InputManager::attachDevice(Input::IInputDevice *dev, Input::Player player)
    {
        if (!dev || dev->type == DEVICE_TYPE_NULL)
        {
            ILOG(szInputFailedFmt, szInput, szDevice, szAttach, szDeviceNull);
            return false;
        }
        _devList.append(dev);
        vpad.m_registerDevice(dev, player);
        ILOG(szInputFmt, dev->name, szDevice, szAttach);
        return 0;
    }

    int InputManager::detachDevice(Input::IInputDevice *dev)
    {
        if (!dev)
        {
            ILOG(szInputFailedFmt, szInput, szDevice, szDetach, szDeviceNull);
            return false;
        }
        _devList.remove(dev);
        vpad.m_unregisterDevice(dev);
        ILOG(szInputFmt, dev->name, szDevice, szDetach, szDeviceNull);
        return 0;
    }

    int InputManager::registerDriver(Input::IInputDriver *driver)
    {
        if (!driver)
        {
            ILOG(szInputFailedFmt, szInput, szDriver, szRegister, szDeviceNull);
            return false;
        }
        _driverList.append(driver);
        ILOG(szInputFmt, driver->getName(), szDriver, szRegister);
        return true;
    }

    int InputManager::unregisterDriver(Input::IInputDriver *driver)
    {
        if (!driver)
        {
            ILOG(szInputFailedFmt, szInput, szDriver, szUnregister, szDeviceNull);
            return false;
        }
        _driverList.remove(driver);
        ILOG(szInputFmt, driver->getName(), szDriver, szUnregister);
        return true;
    }

    const char *InputManager::deviceName(size_t idx)
    {
        if (idx >= _devList.length()) return nullptr;
        return _devList.at(idx)->name;
    }

    const int InputManager::devicePlayer(size_t idx)
    {
        if (idx >= _devList.length()) return 255;
        return static_cast<int>(_devList.at(idx)->player) + 1;
    }
} // namespace Input