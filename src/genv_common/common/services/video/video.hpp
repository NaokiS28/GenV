/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * gpu.hpp - Created on 27-04-2025
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

#include "common/objects/font.hpp"

namespace Video
{
    // size_t msToFrames(size_t millis); // Returns how many frames should elapse within a given time
    // size_t frames();                  // Returns the amount of frames that have been rendered

    inline int getRGB565Length(uint16_t w, uint16_t h)
    {
        return ((sizeof(uint16_t) * w) * h);
    }

    inline int getRGB888Length(uint16_t w, uint16_t h)
    {
        return (((sizeof(uint32_t) - 1) * w) * h);
    }

    inline int getARGBLength(uint16_t w, uint16_t h)
    {
        return ((sizeof(uint32_t) * w) * h);
    }

    enum GPUGradientMode : uint32_t
    {
        GPU_GRADIENT_H = 0,
        GPU_GRADIENT_V = 1,
        GPU_GRADIENT_Ds
    };

    // Runtime-friendly (assumes tightly packed)
    int toRGB565(void *dest, const void *src, size_t pixelCount);
    int toRGB888(void *dest, const void *src, size_t pixelCount);

    // Toolchain-friendly (handles stride/padding)
    int toRGB565_stride(
        void *dest, size_t destStride,
        const void *src, size_t srcStride,
        size_t width, size_t height);

    int toRGB888_stride(
        void *dest, size_t destStride,
        const void *src, size_t srcStride,
        size_t width, size_t height);

    size_t getStringWidth(Fonts::FontObject *fObj, const char *str);

} // namespace Video