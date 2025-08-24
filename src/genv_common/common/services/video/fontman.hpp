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

#include "common/objects/font.hpp"

namespace Fonts
{
    class FontManager
    {
    private:
        FontObject *fontList = nullptr;
        int fontListLength = 0;

    public:
        FontManager();

        int loadFont(FontMetrics font);
        int loadFontFromFile(const char *filePath);
        int loadFontFromMemory();
    };
} // namespace Fonts