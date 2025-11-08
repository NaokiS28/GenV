/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * GVSS.CPP - Created on 10-06-2025
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
#include "common/util/hash.hpp"
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
    constexpr const util::Hash GENV_LOGO_HASH = "GenVLogo"_h;

    GVSS::GVSS(IAppHost *host, Application *appToLoad) : LoadScreenApp(host, appToLoad)
    {
        logo = Sprites::createSprite(
            GENV_LOGO_HASH,
            Textures::openImageMemory(
                GENV_LOGO_HASH,
                Genv_PNG_type,
                genv_logo_data,
                genv_logo_length));

        setAppState(APP_STATE_INIT);
        reload();
    }

    int GVSS::init()
    {
        if (logo)
        {
            logo->uploadTexture();
            fadeIn.setValue(Video::frames(), 0, 255, Video::msToFrames(iFadeTime), Util::TWEEN_STOP);
            fadeOut.setValue(Video::frames(), 255, 0, Video::msToFrames(iFadeTime), Util::TWEEN_STOP);
            reload();
            setAppState(APP_STATE_RUN);
        }
        else
        {
            m_host->requestError(
                "GVSS",
                "Main logo Sprite Object is nullptr.",
                1,
                EM_STYLE_ERROR, EM_ICON_ERROR);
            setAppState(APP_STATE_ERROR);
        }
        return 0;
    }

    void GVSS::render()
    {
        if (getState() == APP_STATE_ERROR) return;
        Video::Color c = Video::Colors::White;
        c.a = alpha;
        // premultiply(c);
        gpu->fillScreen(Video::Colors::Black);
        logo->draw(logoPos.x, logoPos.y);
        gpu->drawText(GVSSText, textPos.x, textPos.y, textPos.w, textPos.h, c, Video::TALIGN_CENTER);
    }

    void GVSS::reload()
    {
        logoPos = {
            static_cast<int>((gpu->getHorizontalRes() / 2) - logo->getTexture()->width) - 30,
            static_cast<int>((gpu->getVerticalRes() / 2) - (logo->getTexture()->height / 2)),
            static_cast<int>(logo->getTexture()->width),
            static_cast<int>(logo->getTexture()->height)};
        textPos = {
            gpu->getHorizontalRes() / 2,
            gpu->getVerticalRes() / 2 - 15,
            500,
            60};
    }

    void GVSS::update()
    {
        switch (GVSSAnimStep)
        {
        case GVSS_FadeIn:
            if (!fadeIn.isDone(Video::frames()) && !fadeIn.isRunning())
                fadeIn.go();
            alpha = fadeIn.getValue(Video::frames());
            if (fadeIn.isDone(Video::frames()))
            {
                GVSSAnimStep = GVSS_Delay;
                timer = System::millis();
            }
            break;
        case GVSS_FadeOut:
            if (!fadeOut.isDone(Video::frames()) && !fadeOut.isRunning())
                fadeOut.go();
            alpha = fadeOut.getValue(Video::frames());
            if (fadeOut.isDone(Video::frames()))
            {
                GVSSAnimStep = GVSS_Exit;
            }
            break;
        case GVSS_Delay:
            if ((System::millis() - timer) >= iTimeToShow)
            {
                GVSSAnimStep = GVSS_FadeOut;
            }
            break;
        case GVSS_Exit:
        default:
            setAppState(APP_STATE_QUIT);
            break;
        }
    }
} // namespace Apps