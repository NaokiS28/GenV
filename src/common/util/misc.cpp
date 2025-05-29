/*
 * Originally from 573in1 - Copyright (C) 2022-2024 spicyjpeg
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

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include "common/util/hash.hpp"
#include "common/util/misc.hpp"
#include "common/util/templates.hpp"

namespace util {

/* Date and time class */

bool Date::isValid(void) const {
	if ((hour > 23) || (minute > 59) || (second > 59))
		return false;
	if ((month < 1) || (month > 12))
		return false;
	if ((day < 1) || (day > getMonthDayCount()))
		return false;

	return true;
}

bool Date::isLeapYear(void) const {
	if (year % 4)
		return false;
	if (!(year % 100) && (year % 400))
		return false;

	return true;
}

int Date::getDayOfWeek(void) const {
	// See https://datatracker.ietf.org/doc/html/rfc3339#appendix-B
	int _year = year, _month = month - 2;

	if (_month <= 0) {
		_month += 12;
		_year--;
	}

	int century = _year / 100;
	_year      %= 100;

	int weekday = 0
		+ day
		+ (_month * 26 - 2) / 10
		+ _year
		+ _year / 4
		+ century / 4
		+ century * 5;

	return weekday % 7;
}

int Date::getMonthDayCount(void) const {
	switch (month) {
		case 2:
			return isLeapYear() ? 29 : 28;

		case 4:
		case 6:
		case 9:
		case 11:
			return 30;

		default:
			return 31;
	}
}

uint32_t Date::toDOSTime(void) const {
	int _year = year - 1980;

	if (!isValid())
		return 0;
	if ((_year < 0) || (_year > 127))
		return 0;

	return 0
		| (_year  << 25)
		| (month  << 21)
		| (day    << 16)
		| (hour   << 11)
		| (minute <<  5)
		| (second >>  1);
}

size_t Date::toString(char *output) const {
	if (!isValid()) {
		*output = 0;
		return 0;
	}

	return sprintf(
		output, "%04d-%02d-%02d %02d:%02d:%02d", year, month, day, hour, minute,
		second
	);
}

void Date::fromCurrentTime(void) {
	auto timeValue = time(nullptr);
	tm   timeObj;

	localtime_s(&timeObj, &timeValue);

	year   = timeObj.tm_year + 1900;
	month  = timeObj.tm_mon  + 1;
	day    = timeObj.tm_mday;
	hour   = timeObj.tm_hour;
	minute = timeObj.tm_min;
	second = timeObj.tm_sec;
}

}