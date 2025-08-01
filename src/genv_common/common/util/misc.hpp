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

namespace util
{

	/* Date and time class */

	class Date
	{
	public:
		uint16_t year;
		uint8_t month, day, hour, minute, second;

		inline void reset(void)
		{
			year = 2024;
			month = 1;
			day = 1;
			hour = 0;
			minute = 0;
			second = 0;
		}

		bool isValid(void) const;
		bool isLeapYear(void) const;
		int getDayOfWeek(void) const;
		int getMonthDayCount(void) const;
		uint32_t toDOSTime(void) const;
		size_t toString(char *output) const;

		void fromCurrentTime(void);
	};

	/* Critical section helper */

	class CriticalSection
	{
	public:
		inline CriticalSection(void)
		{
			// TODO: implement
		}
		inline ~CriticalSection(void)
		{
			// TODO: implement
		}
	};

	template <typename T>
	void ClearMemory(T *dest, size_t len)
	{
		memset(dest, 0, len);
	}

	// Sum a simple 16-bit checksum.
	uint16_t checksum(const void *data, size_t length);
}