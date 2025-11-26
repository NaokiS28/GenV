/*
 * Originally from 573in1 - Copyright (C) 2022-2024 spicyjpeg
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

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "common/util/ifloat.hpp"

namespace util
{
    /* Misc. template utilities */
    template <typename T>
    static constexpr inline uint32_t sum(
        const T *data,
        size_t length)
    {
        uint32_t value = 0;

        for (; length > 0; length--)
            value += uint32_t(*(data++));

        return value;
    }

    template <typename T>
    static constexpr inline T bitwiseXOR(
        const T *data,
        size_t length)
    {
        T value = 0;

        for (; length > 0; length--)
            value ^= *(data++);

        return value;
    }

    template <typename T>
    static constexpr inline bool isEmpty(
        const T *data,
        size_t length,
        T value = 0)
    {
        for (; length > 0; length--)
        {
            if (*(data++) != value)
                return false;
        }

        return true;
    }

    template <typename T>
    static constexpr inline T min(T a, T b)
    {
        return (a < b) ? a : b;
    }

    template <typename T>
    static constexpr inline T max(T a, T b)
    {
        return (a > b) ? a : b;
    }

    template <typename T>
    static constexpr inline T clamp(
        T value,
        T minValue,
        T maxValue)
    {
        if (value < minValue)
            return minValue;
        if (value > maxValue)
            return maxValue;

        return value;
    }

    template <typename T>
    static constexpr inline T rotateLeft(T value, int amount)
    {
        return T((value << amount) | (value >> (sizeof(T) * 8 - amount)));
    }

    template <typename T>
    static constexpr inline T rotateRight(T value, int amount)
    {
        return T((value >> amount) | (value << (sizeof(T) * 8 - amount)));
    }

    // These shall only be used with unsigned types.
    template <typename T>
    static constexpr inline T truncateToMultiple(
        T value,
        T length)
    {
        return value - (value % length);
    }

    template <typename T>
    static constexpr inline T roundUpToMultiple(
        T value,
        T length)
    {
        T diff = value % length;

        if (diff)
            value += length - diff;

        return value;
    }

    template <typename T, typename X>
    static inline void assertAligned(X *ptr)
    {
        assert(!(uintptr_t(ptr) % alignof(T)));
    }

    template <typename T>
    static inline void clear(T &obj, uint8_t value = 0)
    {
        memset(&obj, value, sizeof(obj));
    }

    template <typename T>
    static inline void copy(T &dest, const T &source)
    {
        memcpy(&dest, &source, sizeof(source));
    }

    template <typename T>
    static constexpr inline size_t alignedSizeOf(void)
    {
        return roundUpToMultiple(sizeof(T), alignof(T));
    }

    template <typename T>
    static constexpr inline size_t countOf(T &array)
    {
        return sizeof(array) / sizeof(array[0]);
    }

    template <typename T>
    static constexpr inline auto *endOf(T &array)
    {
        return &array[countOf(array)];
    }

    /* Concatenation and BCD conversion */

    template <typename T, typename V, typename... A>
    static constexpr inline T concat(V value, A... next)
    {
        constexpr int remaining = sizeof...(next);

        if constexpr (remaining)
            return 0 | T(value) | (concat<T>(next...) << (remaining ? (sizeof(V) * 8) : 0));

        return T(value);
    }

    template <typename T, typename V, int N = sizeof(T) / sizeof(V)>
    static constexpr inline T mirror(V value)
    {
        constexpr int remaining = max(N - 1, 0);

        if constexpr (remaining)
            return 0 | T(value) | (mirror<T, V, remaining>(value) << (sizeof(V) * 8));

        return T(value);
    }

    static constexpr inline uint16_t concat2(uint8_t low, uint8_t high)
    {
        return concat<uint16_t>(low, high);
    }
    static constexpr inline uint16_t mirror2(uint8_t value)
    {
        return mirror<uint16_t>(value);
    }

    static constexpr inline uint32_t concat4(uint16_t low, uint16_t high)
    {
        return concat<uint32_t>(low, high);
    }
    static constexpr inline uint32_t mirror4(uint16_t value)
    {
        return mirror<uint32_t>(value);
    }

    static constexpr inline uint32_t concat4(
        uint8_t a,
        uint8_t b,
        uint8_t c,
        uint8_t d)
    {
        return concat<uint32_t>(a, b, c, d);
    }
    static constexpr inline uint32_t mirror4(uint8_t value)
    {
        return mirror<uint32_t>(value);
    }

    static constexpr inline uint8_t encodeBCD(uint8_t value)
    {
        // output = units + tens * 16
        //        = units + tens * 10 + tens * 6
        //        = value             + tens * 6
        return value + (value / 10) * 6;
    }

    static constexpr inline uint8_t decodeBCD(uint8_t value)
    {
        // output = low + high * 10
        //        = low + high * 16 - high * 6
        //        = value           - high * 6
        return value - (value >> 4) * 6;
    }

    static constexpr inline size_t map(size_t x, size_t in_min, size_t in_max, size_t out_min, size_t out_max)
    {
        return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }

    static constexpr inline uint8_t toPercent(size_t val, size_t max)
    {
        if (val > max) val = max;
        return map(val, 0, max, 0, 100);
    }

    static constexpr inline size_t percentOf(uint8_t percent, size_t max)
    {
        if (percent > 100) percent = 100;
        return map(percent, 0, 100, 0, max);
    }

    static constexpr inline size_t scaleP(uint16_t percent, size_t val)
    {
        size_t factor = (percent / 100);
        return (map((percent % 100), 0, 100, 0, val) + (val * factor));
    }

    static constexpr inline ifloat scaleF(ifloat factor, size_t val)
    {
        size_t percent = factor * 100;
        return scaleP(percent, val);
    }

    /* Simple "smart" pointer */

    class Data
    {
    public:
        void *ptr;
        size_t length;

        inline Data(void)
            : ptr(nullptr), length(0) {}
        inline ~Data(void)
        {
            destroy();
        }

        template <typename T>
        inline T *as(void)
        {
            return reinterpret_cast<T *>(ptr);
        }
        template <typename T>
        inline const T *as(void) const
        {
            return reinterpret_cast<const T *>(ptr);
        }
        template <typename T>
        inline T *allocate(size_t count = 1)
        {
            return reinterpret_cast<T *>(allocate(sizeof(T) * count));
        }

        inline void *allocate(size_t _length)
        {
            if (ptr)
                delete[] as<uint8_t>();

            ptr    = _length ? (new uint8_t[_length]) : nullptr;
            length = _length;

            return ptr;
        }
        inline void destroy(void)
        {
            if (ptr)
            {
                delete[] as<uint8_t>();
                ptr = nullptr;
            }
        }
    };

    /* Simple list */
    template <typename T, size_t initialSize>
    class PointerList
    {
    private:
        T *_list           = nullptr;
        size_t _listSize   = 0;
        size_t _listLength = 0;
        bool _ready        = false;

    public:
        PointerList() { _ready = (init() == 0); }
        ~PointerList() { delete[] _list; }

        bool ready() { return _ready; }
        size_t length() { return _listLength; }

        int insert(T ptr, size_t idx)
        {
            if (_listLength >= _listSize && expand(1))
                return 1;
            memmove(&_list[idx + 1], &_list[idx], sizeof(T) * (_listLength - idx));
            _list[idx] = ptr;
            return 0;
        }

        T at(size_t idx)
        {
            if (idx >= _listLength) return nullptr;
            return _list[idx];
        }

        int init()
        {
            _list = new T[initialSize];
            if (!_list)
                return 1;
            memset(_list, 0, sizeof(T) * initialSize);
            _listSize = initialSize;
            return 0;
        }

        int expand(int count)
        {
            auto tempArr = new T[_listSize + count];
            if (!tempArr)
                return 1;

            memset(tempArr, 0, sizeof(T) * (_listSize + count));
            memcpy(tempArr, _list, sizeof(T) * _listSize);
            delete[] _list;
            _list = tempArr;
            _listSize += count;
            return 0;
        }

        int shrink(int count)
        {
            int _c = 0;
            for (size_t i = 0; i < _listSize; i++)
            {
                if (_list[i] != nullptr) _c++;
            }

            if (_c >= (_listSize - count))
                return 2;

            auto tempArr = new T *[_listSize - count];
            if (!tempArr)
                return 1;

            memcpy(tempArr, _list, sizeof(T) * (_listSize - count));
            delete[] _list;
            _list = tempArr;
            _listSize -= count;
            return 0;
        }

        int append(T ptr)
        {
            if (_listLength >= _listSize && expand(1))
                return -1;

            _list[_listLength] = ptr;
            _listLength++;
            return _listLength;
        }

        int remove(size_t idx)
        {
            if (idx >= _listLength)
                return 1;

            _list[idx] = nullptr;
            memmove(&_list[idx], &_list[idx + 1], sizeof(T) * (_listLength - idx - 1));
            _listLength--;

            return 0;
        }

        int remove(T item)
        {
            for (size_t i = 0; i < _listLength; i++)
            {
                if (item == _list[i])
                {
                    return remove(i);
                }
            }
            return 1;
        }

        T *begin() { return _list; }
        T *end() { return _list + _listLength; }
    };

    /* Simple ring buffer */

    template <typename T, size_t N>
    class RingBuffer
    {
    private:
        T _items[N];
        size_t _head, _tail;

    public:
        size_t length;

        inline RingBuffer(void)
            : _head(0), _tail(0), length(0) {}

        inline T *pushItem(void)
        {
            if (length >= N)
                return nullptr;

            auto i = _tail;
            _tail  = (i + 1) % N;
            length++;

            return &_items[i];
        }
        inline T *popItem(void)
        {
            if (!length)
                return nullptr;

            auto i = _head;
            _head  = (i + 1) % N;
            length--;

            return &_items[i];
        }
        inline T *peekItem(void) const
        {
            if (!length)
                return nullptr;

            return &_items[_head];
        }
    };

    /* Simple popularity buffer.
    The least poplular item will be at the start of the list where as more popularly used items
    will remain near the end of the list. When more space is needed, then the least popular items
    will be removed.
    */

    template <typename T, size_t N>
    class PopList
    {
    private:
        T _items[N];
        size_t _top;

    public:
        size_t length;

        inline PopList(void)
            : _top(0), length(0) {}

        // "Use" a member, thus making it climb in popularity if it can
        inline T used(T item)
        {
            if (!length)
                return {};

            for (auto entry : _items)
            {
                if (item == entry)
                {
                    promote(item);
                    return {};
                }
            }

            // Didn't find entry
            return add(item);
        }

        // Directly make a member more popular
        inline void promote(T item)
        {
            if (!length)
                return;

            size_t idx = 0;
            for (auto entry : _items)
            {
                if (item == entry)
                {
                    if (idx < _top)
                    {
                        _items[idx]     = _items[idx + 1];
                        _items[idx + 1] = item;
                    }
                    return;
                }
                else
                    idx++;
            }
        }

        // Directly make a member less popular
        inline void demote(T item)
        {
            if (!length)
                return;

            int idx = 0;
            for (auto entry : _items)
            {
                if (item == entry)
                {
                    if (idx < _top)
                    {
                        _items[idx]     = _items[idx - 1];
                        _items[idx - 1] = item;
                    }
                    return;
                }
                else
                    idx++;
            }
        }

        // Add a new member to the list
        // New members are always popular (lets choose to ignore social conventions)
        inline T add(T item)
        {
            T temp{};
            if (_top >= length)
            {
                temp = remove();
            }
            _items[_top] = item;
            _top++;
            return temp;
        }

        // Removes the least popular member. Returns the value for cases where memory deletes
        //   need to happen.
        inline T remove(void)
        {
            return remove(_items[0]);
        }

        // Removes a specific member. Returns the value for cases where memory deletes
        //   need to happen.
        inline T remove(T item)
        {
            if (!length)
                return {};

            int idx    = 0;
            bool found = false;
            for (auto entry : _items)
            {
                if (item == entry)
                {
                    found = true;
                    break;
                }
                else
                    idx++;
            }

            if (!found)
                return {};

            T temp = _items[idx];
            memmove(_items + (sizeof(T) * idx), _items + (sizeof(T) * (idx + 1)), sizeof(T));
            _top--;
            length--;
            return temp;
        }
    };

} // namespace util

/* Character concatenation operator */

static constexpr inline uint32_t operator""_c(
    const char *const literal,
    size_t length)
{
    return util::concat4(
        (length > 0) ? literal[0] : 0,
        (length > 1) ? literal[1] : 0,
        (length > 2) ? literal[2] : 0,
        (length > 3) ? literal[3] : 0);
}