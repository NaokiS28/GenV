/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * fontman.hpp - Created on 21-08-2025
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

#include "common/objects/font.hpp"
#include "common/objects/texture.hpp"
#include "common/services/adminkey.hpp"
#include "common/util/hash.hpp"

namespace Fonts
{
    enum : uint8_t
    {
        FM_OKAY,
        FM_NOT_FOUND,
        FM_LIST_EMPTY,
        FM_CREATE_OBJECT_FAILED,
        FM_PARAMETER_ERROR,
        FM_OUT_OF_MEMORY,
    };

    class FontManager
    {
    private:
        FontObject **fontList = nullptr;
        int fontListLength = 0;

        int _expandList(int count);
        int _shrinkList(int count);

        int _loadFont(FontObject *fObj);

    public:
        FontManager(AdminClass_Key key);

        int init();
        void shutdown();

        int loadFontFromFile(const char *filePath);
        int loadFontFromMemory(Textures::TextureObject *tObj, const FontMetrics *metrics);
        int loadFontFromMemory(const uint8_t *data, const size_t length, const FontMetrics *metrics);

        int unloadFontAt(const uint8_t idx);
        int unloadFont(util::Hash id);
    };
} // namespace Fonts