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

#include "gvss.hpp"
#include "genvlogo.hpp"

#include "app/app.hpp"
#include "common/formats/image_file.hpp"
#include "common/formats/typenames.hpp"
#include "common/objects/sprite.hpp"
#include "common/objects/texture.hpp"
#include "common/services/services.hpp"
#include "common/util/tween.hpp"

namespace Apps
{
    TMSS::TMSS() : LoadScreenApp()
    {
        setAppState(APP_STATE_INIT);
        reload();
    }

    TMSS::TMSS(Application *appToLoad) : LoadScreenApp()
    {
        this->setAppToLoad(appToLoad);
        setAppState(APP_STATE_INIT);
        reload();
    }

    int TMSS::init(IAppHost *host)
    {
        Textures::TextureObject *tObj = Textures::openImageMemory("GenVLogo"_h, Genv_GIF_type, genv_logo_data, genv_logo_length);
        if (tObj)
        {
            logo = Sprites::createSprite("GenVLogo"_h, tObj);
            logo->uploadTexture();
        }
        fadeIn.setValue(Video::frames(), 0, 255, Video::msToFrames(iFadeTime), Util::TWEEN_STOP);
        fadeOut.setValue(Video::frames(), 255, 0, Video::msToFrames(iFadeTime), Util::TWEEN_STOP);
        reload();
        m_host = host;
        setAppState(APP_STATE_RUN);
        return 0;
    }

    void TMSS::render()
    {
        Video::Color c = Video::Colors::White;
        c.a = alpha;
        // premultiply(c);
        gpu->fillScreen(Video::Colors::Black);
        logo->draw(16, 50, 128, 128);
        gpu->drawText(tmssText, textPos.x, textPos.y, textPos.w, textPos.h, c, Video::TALIGN_CENTER);
    }

    void TMSS::reload()
    {
        textPos = {
            gpu->getHorizontalRes() / 2,
            gpu->getVerticalRes() / 2 - 30,
            500,
            60};
    }

    void TMSS::update()
    {
        switch (tmssAnimStep)
        {
        case TMSS_FadeIn:
            if (!fadeIn.isDone(Video::frames()) && !fadeIn.isRunning())
                fadeIn.go();
            alpha = fadeIn.getValue(Video::frames());
            if (fadeIn.isDone(Video::frames()))
            {
                tmssAnimStep = TMSS_Delay;
                timer = System::millis();
            }
            break;
        case TMSS_FadeOut:
            if (!fadeOut.isDone(Video::frames()) && !fadeOut.isRunning())
                fadeOut.go();
            alpha = fadeOut.getValue(Video::frames());
            if (fadeOut.isDone(Video::frames()))
            {
                tmssAnimStep = TMSS_Exit;
            }
            break;
        case TMSS_Delay:
            if ((System::millis() - timer) >= iTimeToShow)
            {
                tmssAnimStep = TMSS_FadeOut;
            }
            break;
        case TMSS_Exit:
        default:
            setAppState(APP_STATE_QUIT);
            break;
        }
    }
} // namespace Apps