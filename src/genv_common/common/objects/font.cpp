/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * font.cpp - Created on 22-08-2025
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

#include "font.hpp"
#include "common/objects/texture.hpp"

namespace Fonts
{
    FontObject::FontObject(util::Hash objectID)
    {
    }

    FontObject::FontObject(util::Hash objectID, const Fonts::FontMetrics *_metrics, Textures::TextureObject *_texture)
    {
        texture = _texture;
        metrics = _metrics;
    }

    FontObject::~FontObject()
    {
        if (texture)
            delete texture;
        if (metrics)
            delete metrics;
    }

    int FontObject::loadFontFromFile(const char *filePath)
    {
        return 0;
    }

    int FontObject::loadFontFromMem(const void *data, const size_t length)
    {
        return 0;
    }

    FontObject *createFontFromFile(const char *filePath)
    {
        FontObject *fObj = nullptr;
        return fObj;
    }

    FontObject *createFontFromFile(const char *filePath, const FontMetrics *metrics)
    {
        if (filePath == nullptr || metrics == nullptr)
            return nullptr;

        Textures::TextureObject *tObj = Textures::createTextureFromFile(metrics->id, filePath);
        if (!tObj) return nullptr;
        return createFontFromMem(tObj, metrics);
    }

    FontObject *createFontFromMem(const uint8_t *data, const size_t length, const FontMetrics *metrics)
    {
        if (data == nullptr || length == 0 || metrics == nullptr)
            return nullptr;

        Textures::TextureObject *tObj = Textures::createTextureFromMem(metrics->id, data, length);
        if (!tObj) return nullptr;
        return createFontFromMem(tObj, metrics);
    }

    FontObject *createFontFromMem(Textures::TextureObject *tObj, const FontMetrics *metrics)
    {
        if (tObj == nullptr || metrics == nullptr)
            return nullptr;

        FontObject *fObj = nullptr;
        if (tObj != nullptr)
        {
            fObj = new FontObject(metrics->id, metrics, tObj);
        }
        return fObj;
    }

} // namespace Fonts