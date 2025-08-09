/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * nullstorage.cpp - Created on 09-08-2025
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

#include "nullstorage.hpp"

namespace Files
{
    NullStorage::NullStorage() : IStorage() {}
    NullStorage::~NullStorage() {}
    bool NullStorage::init() { return true; }
    bool NullStorage::reset() { return true; }
    void NullStorage::shutdown() { return; }
    uint8_t NullStorage::getDriveList(IStorageDevice *list) { return 0; }
    const char *NullStorage::getWorkingDirectory() { return nullptr; }
}