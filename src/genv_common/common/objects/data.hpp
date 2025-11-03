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
// #include <stdexcept>
#include <stddef.h>
#include <stdint.h>

#include "object.hpp"

class DataObject
{
public:
    DataObject() {}

    DataObject(const uint8_t *data, size_t length)
    {
        _keepDataPtr = true;
        _length = length;
        this->_data = data;
    }

    ~DataObject()
    {
        if (!_keepDataPtr)
            delete[] _data;
    }

    template <typename T>
    T readAs()
    {
        T result{};
        if (!_data || (_length - _position) < sizeof(T)) // Ensure there's enough _data left
            return result;

        memcpy(&result, _data + _position, sizeof(T));
        _position += sizeof(T);
        if (_position >= _length)
            _position = 0;

        return result;
    }

    template <typename T>
    T readAs(size_t position)
    {
        T result{};
        seek(position);

        if (!_data || (_length - _position) < sizeof(T)) // Ensure there's enough _data left
            return result;

        memcpy(&result, _data + _position, sizeof(T));
        _position += sizeof(T);
        if (_position >= _length)
            _position = 0;

        return result;
    }

    template <typename T>
    bool readAs(T *buffer, size_t _length, size_t bufferSize)
    {
        if (!buffer || !_data)
            return false;

        // Ensure there's enough _data left
        int unitLen = sizeof(T);
        if ((_length - _position) < bufferSize)
            return false;

        if (_length > bufferSize)
            return false;

        memcpy(&_data, _data + _position, (unitLen * _length));
        _position += (unitLen * _length);
        if (_position >= _length)
            _position = 0;

        return true;
    }

    template <typename T>
    T peekAs()
    {
        T result{};
        if (!_data)
            return result;

        if ((_length - _position) < sizeof(T))
            return false;

        memcpy(&result, _data + _position, sizeof(T));
        return result;
    }

    template <typename T>
    bool peekAs(T *buffer, size_t _length, size_t bufferSize)
    {
        if (!buffer || !_data)
            return false;

        if ((_length - _position) < bufferSize)
            return false;

        if (_length > bufferSize)
            return false;

        memcpy(&buffer, _data + _position, (bufferSize * _length));
        return true;
    }

    uint8_t peek();
    uint8_t read();
    size_t read(uint8_t *buffer, size_t _length, size_t bufferSize);

    size_t seek(size_t _position);
    size_t set(size_t _position);
    size_t rewind(size_t bytes);
    size_t skip(size_t bytes);

    inline size_t rewind()
    {
        _position = 0;
        return 0;
    }
    inline size_t position() { return _position; }
    inline size_t available() { return (_length - _position); }
    inline size_t size() const { return _length; }

    uint8_t operator[](uint8_t idx) const { return _data[idx]; };

protected:
    const uint8_t *getRawData() { return _data; }

private:
    bool _keepDataPtr = false;
    size_t _position = 0;
    size_t _length = 0;
    const uint8_t *_data = nullptr;
};