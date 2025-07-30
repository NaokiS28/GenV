/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * data.hpp - Created on 27-05-2025
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
#include <string.h>
//#include <stdexcept>
#include <stddef.h>
#include <stdint.h>

#include "object.hpp"
#include "common/services/storage.hpp"

class DataObject : public ObjectBase
{
public:
    DataObject(size_t length) : ObjectBase() {
        this->data = new uint8_t[length];
        if(data != nullptr){
            memset(data, 0, length);
            this->length = length;
        }
    }
    ~DataObject()
    {
        delete[] data;
    }

    template<typename T>
    T* as() {
        //if (sizeof(T) > length) throw std::runtime_error("Insufficient size for type.");
        return reinterpret_cast<T*>(data);
    }

    template<typename T>
    const T* as() const {
        //if (sizeof(T) > length) throw std::runtime_error("Insufficient size for type.");
        return reinterpret_cast<const T*>(data);
    }

    template<typename T>
    void set(const T& value) {
        //if (sizeof(T) > length) throw std::runtime_error("Insufficient size for type.");
        memcpy(data, &value, sizeof(T));
    }

    uint8_t* getRawData() { return data; }
    size_t getDataLen() { return length; }

    private:
    size_t length = 0;
    uint8_t *data = nullptr;
};