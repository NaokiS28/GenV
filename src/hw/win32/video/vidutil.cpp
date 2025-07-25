/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * vidutil.cpp - Created on 22-05-2025
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

#include "common/services/video/video.hpp"
#include "vidutil.hpp"
#include <cstring>

const char *GetDisplayName(LPCSTR devName, DWORD devNum){
    DISPLAY_DEVICE dd;
    dd.cb = sizeof(dd);
    EnumDisplayDevices(devName, 0, &dd, 0);
    UINT len = strlen(dd.DeviceString);
    char *str = new char[len + 1];
    ZeroMemory(str, len+1);
    if(!str)
        return "Unknown Display Name";
    strncpy(str, dd.DeviceString, len);
    return str;
}