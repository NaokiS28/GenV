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

#include "video.hpp"
#include "common/services/services.hpp"

namespace Video
{
    size_t msToFrames(size_t millis)
    {
        size_t framerate = Services::getVideo()->getRefreshRate();

        if (millis < 17)
            millis = 17;
        if (framerate < 20 || framerate > 480)
            framerate = 60;

        size_t msPerFrame = (1000 / framerate);
        return (millis / msPerFrame);
    }

    size_t frames()
    {
        return Services::getVideo()->getFrameCount();
    }

    uint16_t getHorizontalRes()
    {
        return Services::getVideo()->getHorizontalRes();
    }
    uint16_t getVerticalRes()
    {
        return Services::getVideo()->getVerticalRes();
    }
    uint16_t getRefreshRate()
    {
        return Services::getVideo()->getRefreshRate();
    }

    // RGB888 -> RGB565 (3 bytes -> 2 bytes per pixel)
    int toRGB565(void *dest, const void *src, size_t pixelCount)
    {
        if (!dest || !src) return -1;

        const uint8_t *in = reinterpret_cast<const uint8_t *>(src);
        uint16_t *out = reinterpret_cast<uint16_t *>(dest);

        for (size_t i = 0; i < pixelCount; i++)
        {
            uint8_t r = in[i * 3 + 0];
            uint8_t g = in[i * 3 + 1];
            uint8_t b = in[i * 3 + 2];
            out[i] = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
        }
        return 0;
    }

    // RGB565 -> RGB888 (2 bytes -> 3 bytes per pixel)
    int toRGB888(void *dest, const void *src, size_t pixelCount)
    {
        if (!dest || !src) return -1;

        const uint16_t *in = reinterpret_cast<const uint16_t *>(src);
        uint8_t *out = reinterpret_cast<uint8_t *>(dest);

        for (size_t i = 0; i < pixelCount; i++)
        {
            uint16_t c = in[i];
            out[i * 3 + 0] = ((c >> 11) & 0x1F) << 3; // R
            out[i * 3 + 1] = ((c >> 5) & 0x3F) << 2;  // G
            out[i * 3 + 2] = (c & 0x1F) << 3;         // B
        }
        return 0;
    }

    // RGB888 -> RGB565 (stride-aware)
    int toRGB565_stride(
        void *dest, size_t destStride,
        const void *src, size_t srcStride,
        size_t width, size_t height)
    {
        if (!dest || !src) return -1;

        const uint8_t *rowIn = reinterpret_cast<const uint8_t *>(src);
        uint8_t *rowOut = reinterpret_cast<uint8_t *>(dest);

        for (size_t y = 0; y < height; y++)
        {
            const uint8_t *in = rowIn;
            uint16_t *out = reinterpret_cast<uint16_t *>(rowOut);

            for (size_t x = 0; x < width; x++)
            {
                uint8_t r = in[x * 3 + 0];
                uint8_t g = in[x * 3 + 1];
                uint8_t b = in[x * 3 + 2];
                out[x] = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
            }

            rowIn += srcStride;
            rowOut += destStride;
        }
        return 0;
    }

    // RGB565 -> RGB888 (stride-aware)
    int toRGB888_stride(
        void *dest, size_t destStride,
        const void *src, size_t srcStride,
        size_t width, size_t height)
    {
        if (!dest || !src) return -1;

        const uint8_t *rowIn = reinterpret_cast<const uint8_t *>(src);
        uint8_t *rowOut = reinterpret_cast<uint8_t *>(dest);

        for (size_t y = 0; y < height; y++)
        {
            const uint16_t *in = reinterpret_cast<const uint16_t *>(rowIn);
            uint8_t *out = rowOut;

            for (size_t x = 0; x < width; x++)
            {
                uint16_t c = in[x];
                out[x * 3 + 0] = ((c >> 11) & 0x1F) << 3; // R
                out[x * 3 + 1] = ((c >> 5) & 0x3F) << 2;  // G
                out[x * 3 + 2] = (c & 0x1F) << 3;         // B
            }

            rowIn += srcStride;
            rowOut += destStride;
        }
        return 0;
    }

} // namespace Video