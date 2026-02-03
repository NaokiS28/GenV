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
#include <stddef.h>
#include <assert.h>

/*
 * Library to facilitate floating point operations on a CPU without an FPU.
 */
#ifdef GENV_NO_FLOAT
struct ifloat
{
private:
    explicit constexpr ifloat(int32_t raw, bool) noexcept : v(raw) {}

public:
    static constexpr int FRAC_BITS = 16;
    static constexpr int32_t SCALE = 1 << FRAC_BITS;

    int32_t v; // raw Q16.16 value

    // constructors
    constexpr ifloat() noexcept : v(0) {}

    // from integer (scaled)
    constexpr ifloat(int i) noexcept : v(i * SCALE) {}

    // from hardware float
    explicit constexpr ifloat(float f) noexcept : v(static_cast<int32_t>(f * SCALE)) {}

    // from size_t
    explicit constexpr ifloat(size_t i) noexcept : v(static_cast<int32_t>(i) * SCALE) {}

    // convert back
    constexpr float toFloat() const noexcept
    {
        return static_cast<float>(v) / SCALE;
    }

    // allow implicit use where needed
    explicit constexpr operator float() const noexcept { return toFloat(); }
    explicit constexpr operator int() const noexcept { return v >> FRAC_BITS; } // truncates toward zero

    // raw‑value factory
    static constexpr ifloat fromRaw(int32_t raw) noexcept
    {
        return ifloat(raw, true);
    }

    // assignment from hardware float
    ifloat &operator=(float f) noexcept
    {
        v = static_cast<int32_t>(f * SCALE);
        return *this;
    }

    ifloat &operator=(double f) noexcept
    {
        v = static_cast<int32_t>(f * SCALE);
        return *this;
    }

    // assignment from hardware float
    ifloat &operator=(int f) noexcept
    {
        v = static_cast<int32_t>(f * SCALE);
        return *this;
    }

    ifloat &operator=(unsigned int f) noexcept
    {
        v = static_cast<int32_t>(f * SCALE);
        return *this;
    }

    // arithmetic operators
    constexpr ifloat operator+(ifloat o) const noexcept
    {
        return fromRaw(v + o.v);
    }
    constexpr ifloat operator-(ifloat o) const noexcept
    {
        return fromRaw(v - o.v);
    }
    constexpr ifloat operator*(ifloat o) const noexcept
    {
        // 64‑bit intermediate to avoid overflow
        int64_t tmp = int64_t(v) * o.v;
        return fromRaw(static_cast<int32_t>(tmp >> FRAC_BITS));
    }
    constexpr ifloat operator/(ifloat o) const noexcept
    {
        assert(o.v != 0 && "division by zero");
        int64_t tmp = (int64_t(v) << FRAC_BITS) / o.v;
        return fromRaw(static_cast<int32_t>(tmp));
    }

    // compound assignment (ifloat)
    ifloat &operator+=(ifloat o) noexcept
    {
        v += o.v;
        return *this;
    }

    ifloat &operator-=(ifloat o) noexcept
    {
        v -= o.v;
        return *this;
    }

    ifloat &operator*=(ifloat o) noexcept
    {
        int64_t tmp = int64_t(v) * o.v;
        v = static_cast<int32_t>(tmp >> FRAC_BITS);
        return *this;
    }

    ifloat &operator/=(ifloat o) noexcept
    {
        assert(o.v != 0 && "division by zero");
        int64_t tmp = (int64_t(v) << FRAC_BITS) / o.v;
        v = static_cast<int32_t>(tmp);
        return *this;
    }

    // compound assignment (float)
    ifloat &operator+=(float f) noexcept { return *this += ifloat(f); }
    ifloat &operator-=(float f) noexcept { return *this -= ifloat(f); }
    ifloat &operator*=(float f) noexcept { return *this *= ifloat(f); }
    ifloat &operator/=(float f) noexcept { return *this /= ifloat(f); }

    // compound assignment (int)
    ifloat &operator+=(int i) noexcept { return *this += ifloat(i); }
    ifloat &operator-=(int i) noexcept { return *this -= ifloat(i); }
    ifloat &operator*=(int i) noexcept { return *this *= ifloat(i); }
    ifloat &operator/=(int i) noexcept
    {
        assert(i != 0 && "division by zero");
        return *this /= ifloat(i);
    }

    // comparisons
    constexpr bool operator<(ifloat o) const noexcept { return v < o.v; }
    constexpr bool operator<=(ifloat o) const noexcept { return v <= o.v; }
    constexpr bool operator>(ifloat o) const noexcept { return v > o.v; }
    constexpr bool operator>=(ifloat o) const noexcept { return v >= o.v; }
    constexpr bool operator==(ifloat o) const noexcept { return v == o.v; }
    constexpr bool operator!=(ifloat o) const noexcept { return v != o.v; }
};

// int on LHS
inline ifloat operator+(int i, ifloat f) noexcept
{
    return ifloat(i) + f;
}
inline ifloat operator-(int i, ifloat f) noexcept
{
    return ifloat(i) - f;
}
inline ifloat operator*(int i, ifloat f) noexcept
{
    return ifloat(i) * f;
}
inline ifloat operator/(int i, ifloat f) noexcept
{
    return ifloat(i) / f;
}

// unsigned / size_t on LHS (explicit narrowing)
inline ifloat operator+(unsigned i, ifloat f) noexcept
{
    return ifloat((int)i) + f;
}
inline ifloat operator-(unsigned i, ifloat f) noexcept
{
    return ifloat((int)i) - f;
}
inline ifloat operator*(unsigned i, ifloat f) noexcept
{
    return ifloat((int)i) * f;
}
inline ifloat operator/(unsigned i, ifloat f) noexcept
{
    return ifloat((int)i) / f;
}

// uint8_t / small ints
inline ifloat operator+(uint8_t i, ifloat f) noexcept
{
    return ifloat((int)i) + f;
}
inline ifloat operator-(uint8_t i, ifloat f) noexcept
{
    return ifloat((int)i) - f;
}
inline ifloat operator*(uint8_t i, ifloat f) noexcept
{
    return ifloat((int)i) * f;
}
inline ifloat operator/(uint8_t i, ifloat f) noexcept
{
    return ifloat((int)i) / f;
}

#else
using ifloat = float;
#endif

ifloat intToiFloat(int value, int maxValue);
