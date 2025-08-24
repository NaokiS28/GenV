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

#include <stddef.h>
#include <stdint.h>

#include "common/util/hash.hpp"
#include "common/util/misc.hpp"
#include "common/util/templates.hpp"

namespace util
{
	uint16_t checksum(const void *data, size_t length)
	{
		uint16_t sum = 0;
		const uint8_t *_data = (const uint8_t *)data;

		for(size_t idx = 0; idx < length; idx++)
		{
			sum += *(_data + idx);
		}
		return sum;
	}

}