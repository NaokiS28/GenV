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
#include "common/services/video/color.hpp"
#include "common/services/video/video.hpp"
#include "common/util/hash.hpp"
#include "genvlogo.hpp"

#include "app/app.hpp"
#include "common/formats/image_file.hpp"
#include "common/formats/typenames.hpp"
#include "common/objects/sprite.hpp"
#include "common/objects/texture.hpp"
#include "common/util/tween.hpp"

#include "common/services/system/system.hpp"

namespace Apps
{
    constexpr const util::Hash GENV_LOGO_V_HASH = "GenVLogo_V"_h;
    constexpr const util::Hash GENV_LOGO_R_HASH = "GenVLogo_R"_h;

    constexpr const uint16_t line_width = 20;

    GVSS::GVSS(IAppHost *host, Application *appToLoad) : LoadScreenApp(host, appToLoad)
    {
        logo_v = Sprites::createSprite(
            GENV_LOGO_V_HASH,
            Textures::openImageMemory(
                GENV_LOGO_V_HASH,
                Genv_PNG_Image_type,
                genv_v_data,
                genv_v_length));

        logo_ring = Sprites::createSprite(
            GENV_LOGO_R_HASH,
            Textures::openImageMemory(
                GENV_LOGO_R_HASH,
                Genv_PNG_Image_type,
                genv_ring_data,
                genv_ring_length));

        setAppState(APP_STATE_INIT);
        reload();
    }

    int GVSS::init()
    {
        if (logo_ring && logo_v)
        {
            logo_ring->uploadTexture();
            logo_v->uploadTexture();
            fadeIn.setValue(Video::frames(), 0, 255, Video::msToFrames(iFadeTime), Util::TWEEN_STOP);
            fadeOut.setValue(Video::frames(), 255, 0, Video::msToFrames(iFadeTime), Util::TWEEN_STOP);
            logoMove.setValue(Video::frames(), 0, 64, Video::msToFrames(iFadeTime), Util::TWEEN_STOP);
            reload();
            setAppState(APP_STATE_RUN);
        }
        else
        {
            m_host->requestError(
                "GVSS",
                "Main logo Sprite Objects are nullptr.",
                1,
                EM_STYLE_ERROR, EM_ICON_ERROR);
            setAppState(APP_STATE_ERROR);
        }
        return 0;
    }

    void GVSS::render()
    {
        if (getState() == APP_STATE_ERROR) return;
        gpu->fillScreen(Video::Colors::Black);
        // logo_ring->draw(logoPos.x, logoPos.y);
        if (drawLine) gpu->drawRect(linePos, Video::Colors::White);
        logo_v->draw(logoVPos.x, logoVPos.y);
        if (vBoxPos.w > 0) gpu->drawRect(vBoxPos, Video::Colors::Black);
        if (ringAlpha > 0) logo_ring->draw(logoRPos.x, logoRPos.y);
        if (textAlpha > 0) gpu->drawText(GVSSText, textPos.x, textPos.y, textPos.w, textPos.h);
        vStep++;
    }

    void GVSS::reload()
    {
        logoRPos = {
            static_cast<int>((gpu->getHorizontalRes() / 2) - (logo_ring->getTexture()->width / 2)),
            static_cast<int>((gpu->getVerticalRes() / 2) - (logo_ring->getTexture()->height / 2)),
            static_cast<int>(logo_ring->getTexture()->width),
            static_cast<int>(logo_ring->getTexture()->height)};
        logoVPos = {
            static_cast<int>((gpu->getHorizontalRes() / 2) - (logo_v->getTexture()->width / 2)),
            static_cast<int>((gpu->getVerticalRes() / 2) - (logo_v->getTexture()->height / 2)),
            static_cast<int>(logo_v->getTexture()->width),
            static_cast<int>(logo_v->getTexture()->height)};
        vBoxPos = logoVPos;
        textPos = {
            gpu->getHorizontalRes() / 2,
            gpu->getVerticalRes() / 2 - 15,
            500,
            60};
        linePos = {
            0,
            logoVPos.y,
            1,
            4};
    }

    void GVSS::update()
    {
        if (pStep == vStep) return;

        switch (GVSSAnimStep)
        {
        case GVSS_Init:
            lineStop     = logoVPos.x;
            GVSSAnimStep = GVSS_Line1;
            break;
        case GVSS_Line1:
            if (linePos.w < line_width)
                linePos.w += 4;
            else
            {
                if ((linePos.x + linePos.w) < logoVPos.x)
                    linePos.x += 4;
                else
                    GVSSAnimStep = GVSS_VReveal;
            }
            break;
        case GVSS_VReveal:
            if (linePos.w >= 4)
            {
                linePos.x += 4;
                linePos.w -= 4;
            }
            else
                drawLine = false;

            if (vBoxPos.w > 0)
            {
                vBoxPos.x += 4;
                vBoxPos.w -= 4;
            }
            else
            {
                linePos.x    = logoVPos.x + logoVPos.w;
                linePos.w    = 1;
                GVSSAnimStep = GVSS_Line2;
            }
            break;
        case GVSS_Line2:
            if (!drawLine) drawLine = true;
            if (linePos.w < line_width)
                linePos.w += 4;
            else
            {
                if ((linePos.x + linePos.w) < gpu->getHorizontalRes())
                    linePos.x += 4;
                else
                    GVSSAnimStep = GVSS_RingReveal;
            }
            break;
        case GVSS_RingReveal:
            if (linePos.w > 0)
            {
                linePos.x += 4;
                linePos.w -= 4;
            }
            if (ringAlpha < 0xFF)
            {
                ringAlpha++;
            }
            else
            {
                logoMove.go();
                GVSSAnimStep = GVSS_LogoMoveLeft;
            }
            break;
        case GVSS_LogoMoveLeft:
            if (logoMove.isDone(Video::frames()))
            {
                GVSSAnimStep = GVSS_TextReveal;
            }
            break;
        case GVSS_TextReveal:
            if (textAlpha < 0xFF)
            {
                textAlpha++;
            }
            else
            {
                GVSSAnimStep = GVSS_Delay;
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