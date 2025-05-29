/*
 * DXUX - Copyright (C) 2025 NaokiS, spicyjpeg
 * color.hpp - Created on 25-05-2025
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

#pragma once
#include <stdint.h>

namespace Video
{
    struct Color
    {
        uint8_t r = 0;
        uint8_t g = 0;
        uint8_t b = 0;
        uint8_t a = 255;

        constexpr Color() = default;

        constexpr Color(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a = 0xFF)
            : r(_r), g(_g), b(_b), a(_a) {}

        // Constructor from ARGB 32-bit value
        constexpr Color(uint32_t argb)
            : r((argb >> 16) & 0xFF),
              g((argb >> 8) & 0xFF),
              b(argb & 0xFF),
              a((argb >> 24) & 0xFF) {}

        constexpr uint32_t toARGB() const
        {
            return (uint32_t(a) << 24) | (uint32_t(r) << 16) | (uint32_t(g) << 8) | uint32_t(b);
        }

        constexpr uint32_t toABGR() const
        {
            return (uint32_t(a) << 24) | (uint32_t(b) << 16) | (uint32_t(g) << 8) | uint32_t(r);
        }

        constexpr uint16_t toRGB565() const
        {
            return uint16_t(
                ((r >> 3) << 11) |
                ((g >> 2) << 5) |
                (b >> 3));
        }
    };

    namespace Colors {
        constexpr Color Transparent = Color(0, 0, 0, 0);
        constexpr Color Black       = Color(0, 0, 0);
        constexpr Color White       = Color(255, 255, 255);
        constexpr Color Red         = Color(255, 0, 0);
        constexpr Color Green       = Color(0, 255, 0);
        constexpr Color Blue        = Color(0, 0, 255);
        constexpr Color LightBlue   = Color(128, 128, 255);
        constexpr Color Yellow      = Color(255, 255, 0);
        constexpr Color Amber       = Color(255, 191, 0);
        constexpr Color Cyan        = Color(0, 255, 255);
        constexpr Color Magenta     = Color(255, 0, 255);
        constexpr Color Gray        = Color(128, 128, 128);
        constexpr Color DarkGray    = Color(64, 64, 64);
        constexpr Color LightGray   = Color(192, 192, 192);
        constexpr Color Orange      = Color(255, 165, 0);
        constexpr Color Brown       = Color(165, 42, 42);
        constexpr Color Purple      = Color(128, 0, 128);
        constexpr Color Pink        = Color(255, 192, 203);
        constexpr Color Lime        = Color(0, 255, 0);
        constexpr Color Navy        = Color(0, 0, 128);
        constexpr Color Teal        = Color(0, 128, 128);
        constexpr Color Olive       = Color(128, 128, 0);
        constexpr Color Maroon      = Color(128, 0, 0);
        constexpr Color Silver      = Color(192, 192, 192);
        constexpr Color Gold        = Color(255, 215, 0);
        constexpr Color Indigo      = Color(75, 0, 130);
        constexpr Color Violet      = Color(238, 130, 238);
        constexpr Color SkyBlue     = Color(135, 206, 235);
        constexpr Color Coral       = Color(255, 127, 80);
        constexpr Color Salmon      = Color(250, 128, 114);
        constexpr Color Khaki       = Color(240, 230, 140);
        constexpr Color Mint        = Color(189, 252, 201);

        constexpr Color Alpha(const Color& c, uint8_t a) {
            return Color(c.r, c.g, c.b, a);
        }        
    }

    // Blend two colors (alpha blend)
    inline Color blend(const Color &src, const Color &dst)
    {
        uint32_t alpha = src.a;
        uint32_t invAlpha = 255 - alpha;

        uint8_t outR = (uint8_t)((src.r * alpha + dst.r * invAlpha) / 255);
        uint8_t outG = (uint8_t)((src.g * alpha + dst.g * invAlpha) / 255);
        uint8_t outB = (uint8_t)((src.b * alpha + dst.b * invAlpha) / 255);

        // Clamping
        int tempA = src.a + dst.a * invAlpha / 255;
        if (tempA < 0)
            tempA = 0;
        else if (tempA > 255)
            tempA = 255;
        uint8_t outA = static_cast<uint8_t>(tempA);

        return Color(outR, outG, outB, outA);
    }

    // Premultiply alpha
    inline void premultiply(Color &c)
    {
        c.r = (uint8_t)((c.r * c.a) / 255);
        c.g = (uint8_t)((c.g * c.a) / 255);
        c.b = (uint8_t)((c.b * c.a) / 255);
    }
}
