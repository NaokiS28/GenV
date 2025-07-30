/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * object.hpp - Created on 30-07-2025
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

#include "object.hpp"

ObjectBase::ObjectBase()
{
    for (int i = 0; i < paramListSize; i++)
    {
        paramList[i].hash = paramEntryNull;
        paramList[i].param = 0;
    }
}

bool ObjectBase::setParam(util::Hash object, size_t param)
{
    uint8_t pos = getParamPos(object);
    if (pos == paramNotFound)
    {
        // Not in list
        for (int i = 0; i < paramListSize; i++)
        {
            if (paramList[i].hash == paramEntryNull)
            {
                paramList[i].hash = object;
                paramList[i].param = param;
                return true;
            }
        }
        return false;
    }
    else
    {
        paramList[pos].hash = object;
        paramList[pos].param = param;
        return true;
    }
}

// Get metadata - Used for file handling
bool ObjectBase::getParam(util::Hash object, size_t &param)
{
    uint8_t pos = getParamPos(object);
    if (pos == paramNotFound)
    {
        param = paramEntryNull;
        return false;
    }
    param = paramList[pos].param;
    return true;
}

uint8_t ObjectBase::getParamPos(util::Hash object)
{
    for (int i = 0; i < paramListSize; i++)
    {
        if (paramList[i].hash == object)
        {
            return i;
        }
    }
    return paramNotFound;
}

bool ObjectBase::deleteParam(util::Hash object)
{
    uint8_t pos = getParamPos(object);
    if (pos != paramNotFound)
    {
        paramList[pos].hash = paramEntryNull;
        paramList[pos].param = 0;
        return true;
    }
    return false;
}