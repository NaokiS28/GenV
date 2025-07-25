/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * tween.hpp - Created on 11-05-2025
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
#include <stddef.h> // for size_t
#include <stdint.h> // for fixed-width types

namespace Util
{

	// Define the resolution of the tweening scale (used for interpolation).
	constexpr size_t TWEEN_UNIT = 1024;

	// Base easing type: static interface for easing functions.
	struct LinearEasing
	{
		static size_t apply(size_t t) { return t; }
	};

	struct QuadInEasing
	{
		static size_t apply(size_t t) { return (t * t) / TWEEN_UNIT; }
	};

	struct QuadOutEasing
	{
		static size_t apply(size_t t) { return TWEEN_UNIT - ((TWEEN_UNIT - t) * (TWEEN_UNIT - t)) / TWEEN_UNIT; }
	};

	// Tween class for animating values from a starting point to a target value over time.
	template <typename T, typename E>
	class Tween
	{
	public:
		// Set the tween with a start, target, and duration.
		void setValue(size_t time, T start, T target, size_t duration);

		// Instantly set the value without animation.
		void setValue(T target);

		// Get the interpolated value at a given time.
		T getValue(size_t time) const;

		inline bool isDone(size_t time) const {
			return time >= _endTime;
		}

		inline Tween(void) {
			setValue(static_cast<T>(0));
		}

		inline Tween(T start) {
			setValue(start);
		}
	
		inline T getTargetValue(void) const {
			return _base + _delta;
		}

		inline void setValue(size_t time, T target, size_t duration) {
			setValue(time, getValue(time), target, duration);
		}
	

	private:
		int _base = 0;		   	// Starting value
		int _delta = 0;		   	// Difference to target
		size_t _startTime = 0;	// When tween started
		size_t _endTime = 0;   	// When the tween completes
		size_t _duration = 0;
	};

}
