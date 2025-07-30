/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * object.hpp - Created on 20-05-2025
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
#include <stdint.h>
#include <string.h>
#include "common/util/hash.hpp"

constexpr const uint8_t paramListSize = 10;
constexpr const uint8_t paramNotFound = UINT8_MAX;
constexpr const util::Hash paramEntryNull = UINT32_MAX;

struct ObjectParameter
{
    util::Hash hash = 0;
    size_t param = 0;
};

class ObjectBase
{
private:
    util::Hash objectID = 0;
    ObjectParameter paramList[paramListSize];

protected:
    inline void setObjectID(const char *str)
    {
        if (str != nullptr)
        {
            objectID = util::hash(str, strlen(str));
        }
    }

    inline void setObjectID(util::Hash hash)
    {
        objectID = hash;
    }

public:
    ObjectBase();
    virtual ~ObjectBase() = default;

    bool setParam(util::Hash object, size_t param);  // Set a parameter value. Will add a new entry if list has none
    bool getParam(util::Hash object, size_t &param); // Get a parameter value
    uint8_t getParamPos(util::Hash object);          // Find a parameter position in list
    bool deleteParam(util::Hash object);             // Deletes a parameter in the list. Returns false is param was not found

    inline util::Hash getObjectID()
    {
        return objectID;
    }
};