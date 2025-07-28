/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * file.cpp - Created on 27-05-2025
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

#include "file.hpp"
#include <dbt.h>
#include "common/util/hash.hpp"

constexpr const util::Hash Win32_FileHandle = "WIN32_FileHandle"_h;


bool WinStorage::registerDevNotification(GUID devGuid, HDEVNOTIFY &hDev)
{
    DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;

    ZeroMemory(&NotificationFilter, sizeof(NotificationFilter));
    NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
    NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    NotificationFilter.dbcc_classguid = devGuid;

    hDev = RegisterDeviceNotification(
        gpuWnd->hWnd,               // events recipient
        &NotificationFilter,        // type of device
        DEVICE_NOTIFY_WINDOW_HANDLE // type of recipient handle
    );

    return (hDev != nullptr);
}

bool WinStorage::init()
{
    registerDevNotification(hddGUID, hhddNotify);
    registerDevNotification(oddGUID, hoddNotify);
    registerDevNotification(fddGUID, hfddNotify);
    return true;
}

uint8_t WinStorage::getDriveList(Files::IStorageDevice *list)
{
    DWORD drives = GetLogicalDrives();
    if (drives == 0)
        return -1;

    uint8_t driveCount = 0;
    char driveLetter = 'A';
    while (drives)
    {
        if (drives & 1)
        {
            DeviceEntry drive(driveLetter);
            drive.type = GetDriveTypeA(drive.rootPath);

            if (drive.type != DRIVE_UNKNOWN && drive.type != DRIVE_NO_ROOT_DIR)
                driveCount++;

            if (drive.type != DRIVE_CDROM && !isFloppy(drive.rootPath[0]))
            {
                // We don't want to cause unnessecary delays so only call this
                //  on non-floppy and non-optical media. We'll cache the result later.
                if (GetVolumeInformationA(
                        drive.rootPath,
                        drive.volumeName,
                        sizeof(drive.volumeName),
                        &drive.serialNumber,
                        &drive.maxComponentLen,
                        &drive.fileSystemFlags,
                        drive.fileSystemName,
                        sizeof(drive.fileSystemName)))
                {
                    deviceList.push_back(drive);
                }
            }
        }
        driveLetter++;
        drives >>= 1;
    }

    return driveCount;
}

// Checks to see if the given drive letter is likely to be a floppy drive.
bool WinStorage::isFloppy(char driveLetter)
{
    char rootPath[] = {driveLetter, ':', '\\', '\0'};
    UINT type = GetDriveTypeA(rootPath);
    return ((driveLetter == 'A' || driveLetter == 'B') && type == DRIVE_REMOVABLE);
}

// Returns the bus type, i.e. ATAPI, ATA, SCSI, USB
bool WinStorage::GetDriveBusType(char driveLetter, STORAGE_BUS_TYPE &outBusType)
{
    char devicePath[] = "\\\\.\\X:"; // Format for CreateFile
    devicePath[4] = driveLetter;

    // Gets the device handle, doesn't "create a file"
    HANDLE hDevice = CreateFileA(devicePath, 0,
                                 FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    if (hDevice == INVALID_HANDLE_VALUE)
        return false;

    STORAGE_PROPERTY_QUERY query = {};
    query.PropertyId = StorageDeviceProperty;
    query.QueryType = PropertyStandardQuery;

    BYTE buffer[1024] = {};
    DWORD bytesReturned;

    BOOL success = DeviceIoControl(hDevice,
                                   IOCTL_STORAGE_QUERY_PROPERTY,
                                   &query, sizeof(query),
                                   &buffer, sizeof(buffer),
                                   &bytesReturned, NULL);

    CloseHandle(hDevice);

    if (!success)
        return false;

    STORAGE_DEVICE_DESCRIPTOR *descriptor = (STORAGE_DEVICE_DESCRIPTOR *)buffer;
    outBusType = descriptor->BusType;

    return true;
}

void WinStorage::hardwareChanged(WPARAM wParam, LPARAM lParam)
{
    // Check the
    if (!lParam)
        return;
    PDEV_BROADCAST_HDR lpdb = (PDEV_BROADCAST_HDR)lParam;
    // DBT_DEVTYP_DEVICEINTERFACE for removable USB/FW devices?

    if (lpdb->dbch_devicetype == DBT_DEVTYP_VOLUME)
    {
        switch (wParam)
        {
        case DBT_DEVICEARRIVAL:
            // Check whether a CD or DVD was inserted into a drive.
            {
                PDEV_BROADCAST_VOLUME lpdbv = (PDEV_BROADCAST_VOLUME)lpdb;
                if (lpdbv->dbcv_flags & DBTF_MEDIA)
                {
                    DWORD unitMask = lpdbv->dbcv_unitmask;
                    for (int i = 0; i < 26; ++i)
                    {
                        if (unitMask & (1 << i))
                        {
                            // wchar_t driveLetter = L'A' + i;
                            //  e.g., L"A:\\"
                        }
                    }
                }
            }
            break;

        case DBT_DEVICEREMOVECOMPLETE:
            // Check whether a CD or DVD was removed from a drive.
            {
                PDEV_BROADCAST_VOLUME lpdbv = (PDEV_BROADCAST_VOLUME)lpdb;
                if (lpdbv->dbcv_flags & DBTF_MEDIA)
                {
                    DWORD unitMask = lpdbv->dbcv_unitmask;
                    for (int i = 0; i < 26; ++i)
                    {
                        if (unitMask & (1 << i))
                        {
                            // wchar_t driveLetter = L'A' + i;
                            //  e.g., L"A:\\"
                        }
                    }
                }
            }
            break;

        default:
            /*
              Process other WM_DEVICECHANGE notifications for other
              devices or reasons.
            */
            break;
        }
    }
}

int WinStorage::openFile(const char *filePath, bool lock, Files::FileObject *fObj) {
    if(!fObj || !filePath)
        return Files::FO_ERROR_BADPARAM;

    HANDLE file = CreateFileA(
        filePath, 
        (GENERIC_READ | (lock ? GENERIC_WRITE : 0)),    // If lock, expect process wants to write to file
        (lock ? 0 : FILE_SHARE_READ),                   // If lock, set file sharing to "none"
        NULL,
        OPEN_EXISTING,                                  // Only open an existing file, do not create a new one   
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if(GetLastError())
        return Files::FO_ERROR_UNKNOWN; // expand this later
    
    DWORD size = GetFileSize(file, NULL);
    if(size == INVALID_FILE_SIZE || GetLastError())
        return Files::FO_ERROR_UNKNOWN;
    
    // In future this wants to be a streaming object if a file is too large to load into RAM
    DataObject *dObj = new DataObject(size);
    if(dObj == nullptr || !dObj->getDataLen()){
        delete dObj;
        return Files::FO_ERROR_BADOBJECT;
    }
    
    // Need to actually pay attention to the error value here in future
    ReadFile(file, dObj->getRawData(), size, 0, NULL);
    fObj->setDataObj(dObj);
    fObj->setParam(Win32_FileHandle, (size_t)file);
    return Files::FO_OKAY; 
}

int WinStorage::closeFile(Files::FileObject *fObj) { 
    if(fObj){
        HANDLE file;
        size_t r;
        if(fObj->getParam(Win32_FileHandle, r)){
            file = (HANDLE)r;
            CloseHandle(file);
        }
        else
            return -1;
    }
    return 0; 
}

int WinStorage::writeFile(Files::FileObject *fObj) { 
    return 0; 
}

int WinStorage::newFile(const char *filePath, const char *filename, Files::FileObject *fObj) { 
    return 0; 
}

int WinStorage::deleteFile(Files::FileObject *fObj) { 
    return 0; 
}

int WinStorage::readFile(size_t offset, size_t length) { 
    return 0; 
}

int WinStorage::renameFile(const char *filename, Files::FileObject *fObj) { 
    return 0; 
}

const char *WinStorage::getWorkingDirectory(){
    // In future, Convert C:/E: etc to hdd0/rdd0:// etc
    char* path = new char[255];
    GetCurrentDirectoryA(255, path);
    return path;
}

void WinStorage::shutdown()
{
    UnregisterDeviceNotification(hhddNotify);
    UnregisterDeviceNotification(hoddNotify);
    UnregisterDeviceNotification(hfddNotify);
}
