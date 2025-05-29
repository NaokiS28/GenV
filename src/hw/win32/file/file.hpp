/*
 * DXUX - Copyright (C) 2025 NaokiS, spicyjpeg
 * file.hpp - Created on 27-05-2025
 *
 * DXUX is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * DXUX is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * DXUX. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <windows.h>
#include <vector>
#include "common/services/storage.hpp"
#include "../window-mgr.hpp"

class WinStorage : public Files::IStorage
{
private:
    struct DeviceEntry
    {
        char rootPath[4] = {'C', ':', '\\', '\0'};
        char volumeName[MAX_PATH + 1] = {0};
        char fileSystemName[MAX_PATH + 1] = {0};
        UINT type = 0;
        DWORD serialNumber = 0;
        DWORD maxComponentLen = 0;
        DWORD fileSystemFlags = 0;
        DeviceEntry(char driveLetter) { rootPath[0] = driveLetter; }
    };

    std::vector<DeviceEntry> deviceList;

    WindowObject *gpuWnd = nullptr;
    HDEVNOTIFY hhddNotify = nullptr;
    HDEVNOTIFY hoddNotify = nullptr;
    HDEVNOTIFY hfddNotify = nullptr;
    GUID hddGUID = {0x4d36e967, 0xe325, 0x11ce,
                    0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18};
    GUID oddGUID = {0x4d36e965, 0xe325, 0x11ce,
                    0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18};
    GUID fddGUID = {0x4d36e980, 0xe325, 0x11ce,
                    0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18};
    
    
    bool registerDevNotification(GUID devGuid, HDEVNOTIFY &hDev);

public:
    bool
    init();
    bool reset() { return 0; }
    void shutdown();

    bool isFloppy(char driveLetter);
    bool GetDriveBusType(char driveLetter, STORAGE_BUS_TYPE &outBusType);
    void hardwareChanged(WPARAM wParam, LPARAM lParam);

    void setWindow(WindowObject *gpuWnd)
    {
        gpuWnd = gpuWnd;
    }

    uint8_t getDriveList(Files::IStorageDevice *list);
};