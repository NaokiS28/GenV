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
#include "common/objects/texture.hpp"

namespace PSX {
    class PSXTextureObject : public Textures::TextureObject
    {
    public:
        PSXTextureObject() : Textures::TextureObject() {}
        uint8_t vramP = 0;      // Texture Page index
        uint8_t vramX = 0;      // Texture x location in px in vram tpage
        uint8_t vramY = 0;      // Texture y location in px in vram tpage

        // TODO: Allow CLUT to exist independantly of texture
        uint8_t clutIdx = 0;    // CLUT index (x) in px. Only used for 4bpp CLUTs
        uint8_t clutLine = 0;   // Line that CLUT lives on in px.
    };

    // Failsafe texture if a texture object fails to load or otherwise cannot be used
    class PSXDefaultTexture : public PSXTextureObject
    {
    public:
        PSXDefaultTexture();
        ~PSXDefaultTexture() override {} // If this isnt overiding the default texture destructor, program will seg fault when closing.
    };
}