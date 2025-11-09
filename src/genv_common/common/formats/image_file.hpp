/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * soundfile.hpp - Created on 07-06-2025
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

#include "common/objects/texture.hpp"

namespace Textures
{
    Textures::TextureObject *loadPNG_memory(util::Hash objectID, const uint8_t *data, const size_t length);
    Textures::TextureObject *loadGIF_memory(util::Hash objectID, const uint8_t *data, const size_t length);
    Textures::TextureObject *openImageFile(util::Hash objectID, const char *filePath);
    Textures::TextureObject *openImageMemory(util::Hash objectID, uint32_t type, const uint8_t *data, const size_t length);
    // Textures::TextureObject *openImageMemory(util::Hash objectID, const uint8_t *data, const size_t size);
} // namespace Textures