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

#include "iostrings.hpp"
#include "devicemgr.hpp"
#include "common/logger/log.hpp"
#include <string.h>

#define ILOG(fmt, ...) LOG("inputman", fmt __VA_OPT__(, ) __VA_ARGS__)

namespace IO
{

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
            ILOG(szInputFailedFmt, szInput, szDriver, szRegister, szDeviceNull);
            return false;
        }

        if (size_t r = driver->init(); r != GV_OK)
        {
            ILOG(szInputErrorFmt, szInput, szDriver, szInit, r);
            return false;
        }

        _driverList.append(driver);
        ILOG(szInputFmt, driver->getName(), szDriver, szRegister);
        return true;
    }

    int DeviceManager::unregisterDriver(IDriver *driver)
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

} // namespace IO