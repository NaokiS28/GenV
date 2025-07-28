/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * tmss.CPP - Created on 10-06-2025
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

#include "tmss.hpp"
#include "common/services/services.hpp"

namespace Apps
{
    TMSS::TMSS()
    {
        this->state = APP_STATE_RUN;
        // appReady = true;
        this->timer = Services::millis();
        reload();
    }

    void TMSS::render()
    {
        gpu->fillScreen(Colors::Black);
        gpu->drawText(tmssText, 56, textPos.x, textPos.y, textPos.w, textPos.h, Colors::White, TALIGN_CENTER);
    }

    void TMSS::reload()
    {
        textPos = {
            gpu->getHorizontalRes() / 2 - 250,
            gpu->getVerticalRes() / 2 - 30,
            500,
            60};
    }

    void TMSS::update()
    {
        if((Services::millis() - timer) >= iTimeToShow){
            this->state = APP_STATE_QUIT;
        }
    }
}