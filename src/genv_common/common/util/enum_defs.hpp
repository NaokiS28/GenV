/*
 * GenV - Copyright (C) 2025 - 2026 NaokiS, spicyjpeg
 * enum_defs.hpp - Created on 21-02-2026
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

#define ENABLE_BITWISE_OPS(T)                                                                \
    constexpr T operator|(T a, T b)                                                          \
    {                                                                                        \
        return static_cast<T>(                                                               \
            static_cast<__underlying_type(T)>(a) |                                           \
            static_cast<__underlying_type(T)>(b));                                           \
    }                                                                                        \
    constexpr T operator&(T a, T b)                                                          \
    {                                                                                        \
        return static_cast<T>(                                                               \
            static_cast<__underlying_type(T)>(a) &                                           \
            static_cast<__underlying_type(T)>(b));                                           \
    }                                                                                        \
    constexpr T operator~(T a)                                                               \
    {                                                                                        \
        return static_cast<T>(~static_cast<__underlying_type(T)>(a));                        \
    }                                                                                        \
    constexpr T &operator|=(T &a, T b)                                                       \
    {                                                                                        \
        return a = a | b;                                                                    \
    }                                                                                        \
    constexpr T &operator&=(T &a, T b)                                                       \
    {                                                                                        \
        return a = a & b;                                                                    \
    }                                                                                        \
    constexpr T operator^(T a, T b)                                                          \
    {                                                                                        \
        return static_cast<T>(                                                               \
            static_cast<__underlying_type(T)>(a) ^                                           \
            static_cast<__underlying_type(T)>(b));                                           \
    }                                                                                        \
    constexpr T &operator^=(T &a, T b)                                                       \
    {                                                                                        \
        return a = a ^ b;                                                                    \
    }                                                                                        \
    constexpr bool operator<(T a, T b)                                                       \
    {                                                                                        \
        return static_cast<__underlying_type(T)>(a) < static_cast<__underlying_type(T)>(b);  \
    }                                                                                        \
    constexpr bool operator<=(T a, T b)                                                      \
    {                                                                                        \
        return static_cast<__underlying_type(T)>(a) <= static_cast<__underlying_type(T)>(b); \
    }                                                                                        \
    constexpr bool operator>(T a, T b)                                                       \
    {                                                                                        \
        return static_cast<__underlying_type(T)>(a) > static_cast<__underlying_type(T)>(b);  \
    }                                                                                        \
    constexpr bool operator>=(T a, T b)                                                      \
    {                                                                                        \
        return static_cast<__underlying_type(T)>(a) >= static_cast<__underlying_type(T)>(b); \
    }                                                                                        \
    constexpr bool operator==(T a, T b)                                                      \
    {                                                                                        \
        return static_cast<__underlying_type(T)>(a) == static_cast<__underlying_type(T)>(b); \
    }                                                                                        \
    constexpr bool operator!=(T a, T b)                                                      \
    {                                                                                        \
        return static_cast<__underlying_type(T)>(a) != static_cast<__underlying_type(T)>(b); \
    }

template <typename T>
constexpr uint32_t asUint32(T val)
{
    return static_cast<uint32_t>(val);
}

template <typename T>
constexpr __underlying_type(T) asUnderlyingType(T val)
{
    return static_cast<__underlying_type(T)>(val);
}