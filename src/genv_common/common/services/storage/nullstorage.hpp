/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * nullstorage.hpp - Created on 09-08-2025
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
#include "iface_storage.hpp"
#include "storage.hpp"

namespace Files {
    class NullStorage : public IStorage
    {
    public:
        NullStorage();
        ~NullStorage();
        bool init() override;
        bool reset() override;
        void shutdown() override;
        uint8_t getDriveList(IStorageDevice *list) override;
        const char *getWorkingDirectory() override;
    };
}