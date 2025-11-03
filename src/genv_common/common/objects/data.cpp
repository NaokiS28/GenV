/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * data.cpp - Created on 24-10-2025
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

#include "data.hpp"

uint8_t DataObject::read()
{
    uint8_t d = _data[_position++];
    if (_position >= _length)
        _position = 0;

    return d;
}

size_t DataObject::read(uint8_t *buffer, size_t _length, size_t bufferSize)
{
    size_t _count = 0;
    if (_length <= bufferSize && buffer)
    {
        for (; _length > 0; _length--)
        {
            buffer[_count] = read();
            _count++;
        }
    }
    return _count;
}

uint8_t DataObject::peek()
{
    if (!_data)
        return UINT8_MAX;

    if (_position + 1 >= _length)
        return UINT8_MAX;

    return _data[_position + 1];
}

size_t DataObject::rewind(size_t bytes)
{
    if (bytes > _position)
        _position = 0;
    else
        _position -= bytes;
    return _position;
}

size_t DataObject::skip(size_t bytes)
{
    if (bytes < _length)
        _position += bytes;
    else
        _position = _length;
    return _position;
}

size_t DataObject::seek(size_t _position)
{
    if (_position < _length)
        this->_position = _position;
    return _position;
}