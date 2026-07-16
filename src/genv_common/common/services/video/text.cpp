/*
 * GenV - Copyright (C) 2025 - 2026 NaokiS, spicyjpeg
 * text.cpp - Created on 02-05-2026
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

namespace Video
{

    enum TextAlign
    {
        TALIGN_LEFT,
        TALIGN_CENTER,
        TALIGN_RIGHT
    };

    size_t getStringWidth(Fonts::FontObject *fObj, const char *str)
    {
        size_t w   = 0;
        size_t len = strlen(str);
        for (int i = 0; i < len; i++)
        {
            char c = str[i];
            if (c == '\0' || c == '\n' || c == '\r') break;
            auto g = fObj->get(c);
            w += g.w;
        }
        return w;
    }

    size_t getLineHieght(Fonts::FontObject *fObj)
    { return fObj->getHeader()->lineSpacing; }

} // namespace Video