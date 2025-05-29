/*
 * DXUX - Copyright (C) 2025 NaokiS, spicyjpeg
 * storage.hpp - Created on 27-05-2025
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

#include <stdint.h>
#include <cstring>

#include "storage.hpp"

namespace Files
{
    bool isVaildFQFP(const char *path, int len)
    {
        if (path || len > 6) // path needs to exist and len needs to allow for xxx://
        {

            // Copy first part of string for testing
            char *test = new char(10);
            if (!test)
                return false;

            util::ClearMemory(test, 10);
            strncpy(test, path, 8);
            test = strtok(test, "/");
            len = strlen(test);

            if (len < 7)
            {
                // Three character path
                if (strncmp(test + 3, "://", 3) != 0 ||
                    getProviderType(path, len) == StorageType::NUL)
                    // Invalid
                    return false;

                return true;
            }
            else if (len == 7)
            {
                // Four character path (xxxI://)
                char i = path[4];
                if ((i < '0' && i > '9') ||
                    strncmp(test + 3, "://", 3) != 0 ||
                    getProviderType(path, len) == StorageType::NUL)
                    return false;

                return true;
            }
            else if (len == 9)
            {
                // 6 character path (xxxI:I://)
                char i1 = path[4];
                char i2 = path[6];
                if (path[5] != ':' ||
                    (i1 < '0' && i1 > '9') ||
                    (i2 < '0' && i2 > '9') ||
                    strncmp(test + 3, "://", 3) != 0 ||
                    getProviderType(path, len) == StorageType::NUL)
                    return false;

                return true;
            }
            else
            {
                // We dont know what encoding scheme is used
                return false;
            }
        }
        return false;
    }

    StorageType getProviderType(const char *path, int len)
    {
        if (path || len > 6) // path needs to exist and len needs to allow for xxx:/
        {
            char *provider = new char(5);
            strncpy(provider, path, 3);
            int c = 0;

            for (int i = 0; i < 3; i++)
            {
                if (provider[i] >= 'A' && provider[i] <= 'Z')
                {
                    provider += 0x20; // Converts to lower case
                }
            }
            for (auto *type : providerTypes)
            {
                if (!strncmp(provider, type, 3))
                {
                    return StorageType(c);
                }
                c++;
            }
        }
        return StorageType::NUL;
    }

    uint8_t getProviderIndex(const char *path, int len)
    {
        if (path || len > 6) // path needs to exist and len needs to allow for xxxI:/
        {
            char c = path[4];
            if (c >= '0' && c <= '9')
            {
                return (c - 0x30); // Convert ASCII to int
            }
        }
        return 0;
    }

    uint8_t getProviderSubIndex(const char *path, int len)
    {
        if (path || len > 8) // path needs to exist and len needs to allow for xxxI:i:/
        {
            char d1 = path[5];
            char i = path[6];
            char d2 = path[7];
            if (d1 == ':' && d2 == ':')
            {
                // Path has sub device index
                if (i >= '0' && i <= '9')
                {
                    return (i - '0'); // Convert ASCII to int
                }
            }
        }
        return 0;
    }

}