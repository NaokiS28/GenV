/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * device_mgr.cpp - Created on 22-08-2025
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

#include <string.h>

#include "device_mgr.hpp"
#include "common/logger/log.hpp"

#define DM_LOG(fmt, ...) LOG("devmgr", fmt __VA_OPT__(, ) __VA_ARGS__)

namespace System
{
    constexpr const char szDriverFailedFmt[] = "%s driver failed to %s: %s";
    // constexpr const char szDriverErrorFmt[]  = "%s driver gave error whilst %s: %08X";
    constexpr const char szDriverFmt[]  = "%s driver %sed.";
    constexpr const char szInit[]       = "initialising";
    constexpr const char szRegister[]   = "register";
    constexpr const char szUnregister[] = "unregister";

    constexpr const char szDeviceNull[] = "device pointer is null.";

    DeviceManager::~DeviceManager()
    {
        for (auto driver : _driverList)
            if (driver) delete driver;
    }

    int DeviceManager::init()
    {
        if (!_driverList.ready() || !_driverList.ready())
            return 1;
        return 0;
    }

    void DeviceManager::update()
    {
        for (auto driver : _driverList)
            if (driver) driver->update();

        // for (auto pad : _driverList)
    }

    void DeviceManager::reset()
    {
        for (auto driver : _driverList)
            if (driver) driver->reset();
    }

    void DeviceManager::shutdown()
    {
        for (auto driver : _driverList)
            if (driver) driver->shutdown();
    }

    int DeviceManager::registerDriver(IDriver *driver)
    {
        if (!driver)
        {
            DM_LOG(szDriverFailedFmt, szRegister, szDeviceNull);
            return false;
        }

        if (size_t r = driver->init(); r != GV_OK)
        {
            DM_LOG(szDriverFailedFmt, szInit, r);
            return false;
        }

        _driverList.append(driver);
        DM_LOG(szDriverFmt, driver->getName(), szRegister);
        return true;
    }

    int DeviceManager::unregisterDriver(IDriver *driver)
    {
        if (!driver)
        {
            DM_LOG(szDriverFailedFmt, szUnregister, szDeviceNull);
            return false;
        }
        _driverList.remove(driver);
        DM_LOG(szDriverFmt, driver->getName(), szUnregister);
        return true;
    }

} // namespace System