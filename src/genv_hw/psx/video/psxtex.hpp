/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * psxtex.hpp - Created on 04-08-2025
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
#include "common/util/hash.hpp"
#include "common/objects/texture.hpp"

namespace System::PSX::GPU
{
    static constexpr util::Hash GENV_PSX_TEXTURE_TYPE_NAME = "GenVPSXTextureObject"_h;

    class PSXTextureObject : public Textures::TextureObject
    {
    public:
        PSXTextureObject(util::Hash objectID);
        PSXTextureObject(util::Hash objectID, const char *filePath);
        ~PSXTextureObject() override;

        uint8_t texPage = 0; // Texture Page index
        uint16_t vramX = 0;  // Texture x location in px in vram tpage
        uint16_t vramY = 0;  // Texture y location in px in vram tpage

        // TODO: Allow CLUT to exist independantly of texture
        uint16_t clutX = 0; // CLUT index (x) in vram in px. Only used for 4bpp CLUTs
        uint16_t clutY = 0; // Line that CLUT lives in vram in px.
    };
} // namespace System::PSX::GPU