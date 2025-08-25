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

FontObject::FontObject(util::Hash objectID)
{
}

FontObject::FontObject(util::Hash objectID, Fonts::FontMetrics *_metrics, Textures::TextureObject *_texture)
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

FontObject::FontObject(util::Hash objectID, const char *filePath) {}

int FontObject::loadFontFromFile(const char *filePath)
{
    return 0;
}

int FontObject::loadFontFromMem()
{
    return 0;
}