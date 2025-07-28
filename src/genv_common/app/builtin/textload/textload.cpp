/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * BasicLoader.CPP - Created on 10-06-2025
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

#include "textload.hpp"
#include "common/services/services.hpp"

namespace Apps
{
    TextLoader::TextLoader()
    {
        state = APP_STATE_RUN;
        reload();
    }

    void TextLoader::render()
    {
        gpu->fillScreen(Colors::Black);
        gpu->drawText(loadingText, 15, textPos.x, textPos.y, textPos.w, textPos.h, Colors::White, TALIGN_CENTER);
    }

    void TextLoader::reload()
    {
        textPos = {
            gpu->getHorizontalRes() / 2 - 250,
            gpu->getVerticalRes() / 2 - 10,
            500,
            20};
    }
}