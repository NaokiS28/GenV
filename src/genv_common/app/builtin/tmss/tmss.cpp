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
#include "common/util/tween.hpp"

namespace Apps
{
    TMSS::TMSS()
    {
        this->state = APP_STATE_INIT;
        reload();
    }

    int TMSS::init()
    {
        fadeIn.setValue(Services::frames(), 0, 255, Services::msToFrames(iFadeTime), Util::TWEEN_STOP);
        fadeOut.setValue(Services::frames(), 255, 0, Services::msToFrames(iFadeTime), Util::TWEEN_STOP);
        reload();
        state = APP_STATE_RUN;
        return 0;
    }

    void TMSS::render()
    {
        Color c = Colors::White;
        c.a = alpha;
        premultiply(c);
        gpu->fillScreen(Colors::Black);
        gpu->drawText(tmssText, 56, textPos.x, textPos.y, textPos.w, textPos.h, c, TALIGN_CENTER);
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
        switch (tmssAnimStep)
        {
        case TMSS_FadeIn:
            if(!fadeIn.isDone(Services::frames()) && !fadeIn.isRunning())
                fadeIn.go();
            alpha = fadeIn.getValue(Services::frames());
            if (fadeIn.isDone(Services::frames()))
            {
                tmssAnimStep = TMSS_Delay;
                timer = Services::millis();
            }
            break;
        case TMSS_FadeOut:
            if(!fadeOut.isDone(Services::frames()) && !fadeOut.isRunning())
                fadeOut.go();
            alpha = fadeOut.getValue(Services::frames());
            if (fadeOut.isDone(Services::frames()))
            {
                tmssAnimStep = TMSS_Exit;
            }
            break;
        case TMSS_Delay:
            if ((Services::millis() - timer) >= iTimeToShow)
            {
                tmssAnimStep = TMSS_FadeOut;
            }
            break;
        case TMSS_Exit:
        default:
            state = APP_STATE_QUIT;
        }
    }
}