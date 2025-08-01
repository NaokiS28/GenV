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

#include "common/util/ifloat.hpp"

namespace util
{

	/* Misc. template utilities */

	template <typename T>
	static inline uint32_t sum(const T *data, size_t length)
	{
		uint32_t value = 0;

		for (; length; length--)
			value += uint32_t(*(data++));

		return value;
	}

	template <typename T>
	static inline T min(T a, T b)
	{
		return (a < b) ? a : b;
	}

	template <typename T>
	static inline T max(T a, T b)
	{
		return (a > b) ? a : b;
	}

	template <typename T>
	static inline T clamp(T value, T minValue, T maxValue)
	{
		return (value < minValue) ? minValue : ((value > maxValue) ? maxValue : value);
	}

	template <typename T>
	static inline T rotateLeft(T value, int amount)
	{
		return T((value << amount) | (value >> (sizeof(T) * 8 - amount)));
	}

	template <typename T>
	static inline T rotateRight(T value, int amount)
	{
		return T((value >> amount) | (value << (sizeof(T) * 8 - amount)));
	}

	// These shall only be used with unsigned types.
	template <typename T>
	static inline T truncateToMultiple(T value, T length)
	{
		return value - (value % length);
	}

	template <typename T>
	static inline T roundUpToMultiple(T value, T length)
	{
		T diff = value % length;
		return diff ? (value - diff + length) : value;
	}

	template <typename T, typename X>
	static inline void assertAligned(X *ptr)
	{
		// assert(!(reinterpret_cast<uintptr_t>(ptr) % alignof(T)));
	}

	template <typename T>
	static inline void clear(T &obj, uint8_t value = 0)
	{
		__builtin_memset(&obj, value, sizeof(obj));
	}

	template <typename T>
	static constexpr inline size_t countOf(T &array)
	{
		return sizeof(array) / sizeof(array[0]);
	}

	template <typename T, typename X>
	static inline T forcedCast(X item)
	{
		return reinterpret_cast<T>(reinterpret_cast<void *>(item));
	}

	static constexpr inline uint16_t concatenate(uint8_t a, uint8_t b)
	{
		return a | (b << 8);
	}

	static constexpr inline uint32_t concatenate(
		uint8_t a, uint8_t b, uint8_t c, uint8_t d)
	{
		return a | (b << 8) | (c << 16) | (d << 24);
	}

	static constexpr inline size_t map(size_t x, size_t in_min, size_t in_max, size_t out_min, size_t out_max)
	{
		return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
	}

	static constexpr inline uint8_t toPercent(size_t val, size_t max)
	{
		if(val > max) val = max;
		return map(val, 0, max, 0, 100);
	}

	static constexpr inline size_t percentOf(uint8_t percent, size_t max)
	{
		if(percent > 100) percent = 100;
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

			ptr = _length ? (new uint8_t[_length]) : nullptr;
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
			_tail = (i + 1) % N;
			length++;

			return &_items[i];
		}
		inline T *popItem(void)
		{
			if (!length)
				return nullptr;

			auto i = _head;
			_head = (i + 1) % N;
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

}
