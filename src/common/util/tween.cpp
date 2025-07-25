/*
 * 573in1 - Copyright (C) 2022-2024 spicyjpeg
 *
 * 573in1 is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * 573in1 is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * 573in1. If not, see <https://www.gnu.org/licenses/>.
 */

#include <assert.h>
#include <stdint.h>
#include "common/util/tween.hpp"

namespace Util
{

	// Configure the tween for a transition: set start, end, and duration.
	template <typename T, typename E>
	void Tween<T, E>::setValue(size_t time, T start, T target, size_t duration)
	{
		_startTime = time;
		_base = start;
		_delta = target - start;
		_endTime = time + duration;
		_duration = duration;
	}

	// Immediately set the value (no animation).
	template <typename T, typename E>
	void Tween<T, E>::setValue(T target)
	{
		_base = target;
		_delta = static_cast<T>(0); // No delta (no interpolation)
		_endTime = 0;				// Disables interpolation
	}

	// Compute the interpolated value at a given time.
	template <typename T, typename E>
	T Tween<T, E>::getValue(size_t time) const
	{
		if (time >= _endTime)
			return _base + _delta;

		size_t elapsed = time - _startTime;
		size_t progress = (elapsed * TWEEN_UNIT) / _duration;

		T interpolated = _base + (_delta * static_cast<T>(E::apply(progress))) / static_cast<T>(TWEEN_UNIT);
		return interpolated;
	}

	// Explicit instantiations for commonly used types and easing functions.
	template class Tween<int, LinearEasing>;
	template class Tween<int, QuadInEasing>;
	template class Tween<int, QuadOutEasing>;
	template class Tween<uint8_t, LinearEasing>;
	template class Tween<uint8_t, QuadInEasing>;
	template class Tween<uint8_t, QuadOutEasing>;
	template class Tween<uint16_t, LinearEasing>;
	template class Tween<uint16_t, QuadInEasing>;
	template class Tween<uint16_t, QuadOutEasing>;
	template class Tween<uint32_t, LinearEasing>;
	template class Tween<uint32_t, QuadInEasing>;
	template class Tween<uint32_t, QuadOutEasing>;

	template class Tween<int, QuadInOutEasing>;
	template class Tween<int, CubicInEasing>;
	template class Tween<int, CubicOutEasing>;
	template class Tween<int, CubicInOutEasing>;
	template class Tween<uint8_t, QuadInOutEasing>;
	template class Tween<uint8_t, CubicInEasing>;
	template class Tween<uint8_t, CubicOutEasing>;
	template class Tween<uint8_t, CubicInOutEasing>;
	template class Tween<uint16_t, QuadInOutEasing>;
	template class Tween<uint16_t, CubicInEasing>;
	template class Tween<uint16_t, CubicOutEasing>;
	template class Tween<uint16_t, CubicInOutEasing>;
	template class Tween<uint32_t, QuadInOutEasing>;
	template class Tween<uint32_t, CubicInEasing>;
	template class Tween<uint32_t, CubicOutEasing>;
	template class Tween<uint32_t, CubicInOutEasing>;

}
