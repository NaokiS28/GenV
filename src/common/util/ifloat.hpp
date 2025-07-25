/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * intfloat.hpp - Created on 03-05-2025
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
#include <stdint.h>
#include <assert.h>

/*
 * Library to facilitate floating point operations on a CPU without an FPU.
*/
#ifdef GFX_NO_FLOAT
struct ifloat {
    static constexpr int FRAC_BITS = 16;
    static constexpr int32_t SCALE = 1 << FRAC_BITS;

    int32_t v;    // raw Q16.16 value

    // constructors
    constexpr ifloat() noexcept : v(0) {}
    // from raw fixed‑point
    explicit constexpr ifloat(int32_t raw) noexcept : v(raw) {}
    // from hardware float
    explicit ifloat(float f) noexcept
      : v(static_cast<int32_t>(f * SCALE))
    {}

    // convert back
    float toFloat() const noexcept {
        return static_cast<float>(v) / SCALE;
    }
    // allow implicit use where a float is needed
    explicit operator float() const noexcept { return toFloat(); }

    // raw‑value factory
    static constexpr ifloat fromRaw(int32_t raw) noexcept {
        return ifloat(raw);
    }

    // arithmetic operators
    constexpr ifloat operator+ (ifloat o) const noexcept {
        return fromRaw(v + o.v);
    }
    constexpr ifloat operator- (ifloat o) const noexcept {
        return fromRaw(v - o.v);
    }
    constexpr ifloat operator* (ifloat o) const noexcept {
        // 64‑bit intermediate to avoid overflow
        int64_t tmp = int64_t(v) * o.v;
        return fromRaw(static_cast<int32_t>(tmp >> FRAC_BITS));
    }
    constexpr ifloat operator/ (ifloat o) const noexcept {
        assert(o.v != 0 && "division by zero");
        int64_t tmp = (int64_t(v) << FRAC_BITS) / o.v;
        return fromRaw(static_cast<int32_t>(tmp));
    }

    // mixed‑type convenience
    ifloat operator+ (float f) const noexcept { return *this + ifloat(f); }
    ifloat operator- (float f) const noexcept { return *this - ifloat(f); }
    ifloat operator* (float f) const noexcept { return *this * ifloat(f); }
    ifloat operator/ (float f) const noexcept { return *this / ifloat(f); }

    // comparisons
    constexpr bool operator< (ifloat o) const noexcept { return v <  o.v; }
    constexpr bool operator<=(ifloat o) const noexcept { return v <= o.v; }
    constexpr bool operator> (ifloat o) const noexcept { return v >  o.v; }
    constexpr bool operator>=(ifloat o) const noexcept { return v >= o.v; }
    constexpr bool operator==(ifloat o) const noexcept { return v == o.v; }
    constexpr bool operator!=(ifloat o) const noexcept { return v != o.v; }
};
#else
using ifloat = float;
#endif

ifloat intToiFloat(int value, int maxValue);
