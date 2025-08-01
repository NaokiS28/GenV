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
	enum : bool
	{
		TWEEN_STOP = false,
		TWEEN_PLAY = true
	};

	// Define the resolution of the tweening scale (used for interpolation).
	constexpr int TWEEN_UNIT = 1024;

	// Base easing type: static interface for easing functions.
	struct LinearEasing
	{
		static int apply(int t) { return t; }
	};

	struct QuadInEasing
	{
		static int apply(int t) { return (t * t) / TWEEN_UNIT; }
	};

	struct QuadOutEasing
	{
		static int apply(int t) { return TWEEN_UNIT - ((TWEEN_UNIT - t) * (TWEEN_UNIT - t)) / TWEEN_UNIT; }
	};

	struct QuadInOutEasing
	{
		static int apply(int t)
		{
			if (t < TWEEN_UNIT / 2)
			{
				return (2 * t * t) / TWEEN_UNIT;
			}
			else
			{
				int u = TWEEN_UNIT - t;
				return TWEEN_UNIT - (2 * u * u) / TWEEN_UNIT;
			}
		}
	};

	struct CubicInEasing
	{
		static int apply(int t)
		{
			return (t * t / TWEEN_UNIT) * t / TWEEN_UNIT;
		}
	};

	struct CubicOutEasing
	{
		static int apply(int t)
		{
			int u = TWEEN_UNIT - t;
			return TWEEN_UNIT - (u * u / TWEEN_UNIT) * u / TWEEN_UNIT;
		}
	};
	struct CubicInOutEasing
	{
		static int apply(int t)
		{
			if (t < TWEEN_UNIT / 2)
			{
				return 4 * t * t * t / (TWEEN_UNIT * TWEEN_UNIT);
			}
			else
			{
				int u = TWEEN_UNIT - t;
				return TWEEN_UNIT - (4 * u * u * u) / (TWEEN_UNIT * TWEEN_UNIT);
			}
		}
	};

	// Tween class for animating values from a starting point to a target value over time.
	template <typename T, typename E>
	class Tween
	{
	public:
		// Set the tween with a start, target, and duration.
		void setValue(size_t time, T start, T target, size_t duration, bool play = true);

		// Instantly set the value without animation.
		void setValue(T target, bool play = true);

		// Get the interpolated value at a given time.
		T getValue(size_t time) const;

		inline void go() { _go = true; }
		inline void stop() { _go = false; }
		inline bool isRunning() { return _go; }

		inline bool isDone(size_t time)
		{
			// If tween is paused, continually updated the timer until it's running again.
			if (!_go)
			{
				_startTime += time;
				_endTime += time;
			}
			return time >= _endTime;
		}

		inline Tween(void)
		{
			setValue(static_cast<T>(0));
		}

		inline Tween(T start)
		{
			setValue(start);
		}

		inline T getTargetValue(void) const
		{
			return _base + _delta;
		}

		inline void setValue(size_t time, T target, size_t duration)
		{
			setValue(time, getValue(time), target, duration);
		}

	private:
		bool _go = true;	   // Tween is playing
		int _base = 0;		   // Starting value
		int _delta = 0;		   // Difference to target
		size_t _startTime = 0; // When tween started
		size_t _endTime = 0;   // When the tween completes
		size_t _duration = 0;
	};

}
